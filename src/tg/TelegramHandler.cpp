#include "TelegramHandler.hpp"

#include <iostream>
#include <optional>
#include <vector>
#include "../tools/StringTools.hpp"
#include "../tools/TimeTools.hpp"
#include "../db/ORMClasses.hpp"

tg::TelegramHandler::TelegramHandler(std::string token, db::DatabaseHandler& db, tg::LanguageHandler& lang_handler)
    : bot{token}, db{db}, lang_handler{lang_handler} {
    for (const auto& user : db.get_storage().get_all<orm::User>()) {
        chat_states[user.telegram_id] = UserChatState(user.telegram_id, user.id, user.language);
    }
    bot.getEvents().onCommand("start", [this](tgb::Message::Ptr message) {
        start_command(message);
    });
    bot.getEvents().onCommand("language", [this](tgb::Message::Ptr message) {
        change_language(message);
    });
    bot.getEvents().onCallbackQuery([this, &db, &lang_handler](tgb::CallbackQuery::Ptr query) {
        try {
            bot.getApi().answerCallbackQuery(query->id);
        } catch (const std::exception& e) {
            std::cerr << "Callback answer failed: " << e.what() << std::endl;
            return;
        }
        std::clog << "CallbackQuery: " << query->data << " from (" << query->message->chat->id << ")" << std::endl;
        auto user_id = get_user_by_tg_id(query->message->chat->id);
        auto data = strtools::split(query->data, ':');
        if (!user_id) {
            send_message(query->message->chat->id, lang_handler.localize("universal.registartion-ask", "en"));
            return;
        }
        auto user_state = get_chat_state(query->message->chat->id);
        if (data.size() >= 2 && data[0] == "v1") {
            if (data[1] == "lang-change") {
                change_language_query(query, data[2]);
            } else {
                send_message(query->message->chat->id, lang_handler.localize("query.unknown[query]", user_state.get_language()));
            }
        } else {
            send_message(query->message->chat->id, lang_handler.localize("query.cant-handle[query]", user_state.get_language()));
        }
    });
}

std::optional<uint64_t> tg::TelegramHandler::get_user_by_tg_id(chat_id_t chat_id) {
    if (auto it = chat_states.find(chat_id); it != chat_states.end()) {
        return {it->second.get_db_id()};
    }
    std::clog << "||Someone's state not found in chat_states for chat_id: " << chat_id << std::endl;
    auto user = db.get_storage().get_all<orm::User>(sql::where(sql::c(&orm::User::telegram_id) == chat_id));
    if (!user.empty()) {
        chat_states[chat_id] = tg::UserChatState{user.front().telegram_id, user.front().id, user.front().language};
        return {user.front().id};
    }
    return std::nullopt;
}

tg::UserChatState& tg::TelegramHandler::get_chat_state(chat_id_t chat_id) {
    if (auto it = chat_states.find(chat_id); it != chat_states.end()) {
        return it->second;
    }
    std::cerr << "||No chat state found for chat_id: " << chat_id << std::endl;
    throw std::runtime_error("No chat state found for chat_id: " + std::to_string(chat_id));
}

void tg::TelegramHandler::run_polling() {
    try {
        std::clog << "Running bot: " << bot.getApi().getMe()->username << std::endl;
        bot.getApi().deleteWebhook();
        tgb::TgLongPoll long_pool{bot};
        while (true) {
            std::clog << "Polling..." << std::endl;
            long_pool.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void tg::TelegramHandler::send_message(tg::chat_id_t chat_id, const tg::markdown_string& text) {
    bot.getApi().sendMessage(chat_id, text.get_text(), nullptr, nullptr, nullptr, "MarkdownV2");
}
void tg::TelegramHandler::send_markdown(tg::chat_id_t chat_id, const tg::markdown_string& text) {
    bot.getApi().sendMessage(chat_id, text.get_text(), nullptr, nullptr, nullptr, "MarkdownV2");
}
void tg::TelegramHandler::send_markdown(tg::chat_id_t chat_id, const tg::markdown_string& text, const tgb::InlineKeyboardMarkup::Ptr& keyboard) {
    bot.getApi().sendMessage(chat_id, text.get_text(), nullptr, nullptr, keyboard, "MarkdownV2");
}

void tg::TelegramHandler::send_message(tg::chat_id_t chat_id, const std::string& text) {
    bot.getApi().sendMessage(chat_id, text);
}
void tg::TelegramHandler::send_markdown(tg::chat_id_t chat_id, const std::string& text) {
    bot.getApi().sendMessage(chat_id, text, nullptr, nullptr, nullptr, "MarkdownV2");
}

void tg::markdown_string::operator+=(const std::string& str) {
    text += markdown_escape(str);
}

template<typename... MDs>
requires (std::is_same_v<MDs, tg::MDMode> && ...)
void tg::markdown_string::add(const std::string_view str, MDs... modes) {
    if constexpr (sizeof...(modes) == 0)
        text += markdown_escape(str);
    else {
        std::string md_str{str};
        ((md_str = markdown_apply(md_str, modes)),...);
        text += md_str;
    }
}

std::string tg::markdown_string::markdown_escape(const std::string_view text) {
    std::string output{};
    output.reserve(2*text.size());
    for (auto chr : text) {
        switch (chr) {
            case '_': case '*': case '[':
            case ']': case '(': case ')':
            case '~': case '`': case '>':
            case '#': case '+': case '-':
            case '=': case '|': case '{':
            case '}': case '.': case '!':
            case '\\':
                output += '\\';
        }
        output += chr;
    }
    return output;
}

std::string tg::markdown_string::markdown_apply(const std::string_view str, MDMode mode)  {
    switch (mode) {
        case MDMode::PLAIN:         return markdown_escape(str);
        case MDMode::BOLD:          return std::string{"*"}   + markdown_escape(str) + "*";
        case MDMode::ITALIC:        return std::string{"_"}   + markdown_escape(str) + "_";
        case MDMode::UNDERLINE:     return std::string{"_"}   + markdown_escape(str) + "_";
        case MDMode::STRIKETHROUGH: return std::string{"~"}   + markdown_escape(str) + "~";
        case MDMode::SPOILER:       return std::string{"||"}  + markdown_escape(str) + "||";
        case MDMode::CODE:          return std::string{"`"}   + markdown_escape(str) + "`";
        case MDMode::TEXT:          return std::string{"```"} + markdown_escape(str) + "```";
    }
    return markdown_escape(str);
}

void tg::TelegramHandler::start_command(tgb::Message::Ptr message) {
    auto user_id = get_user_by_tg_id(message->chat->id);
    tg::markdown_string md_str{};
    if (user_id) {
        auto user_state = get_chat_state(message->chat->id);
        md_str.add(lang_handler.localize("start.welcome[BOLD]", user_state.get_language()), tg::MDMode::BOLD);
        send_markdown(message->chat->id, md_str);
        md_str.clear();
        md_str.add(lang_handler.localize("start.registrated-id[1]", user_state.get_language()));
        md_str.add(lang_handler.localize("start.registrated-id[2|ITALIC]", user_state.get_language()), tg::MDMode::ITALIC);
        md_str.add(std::to_string(user_state.get_db_id()), tg::MDMode::CODE);
        send_markdown(message->chat->id, md_str);
        std::clog << "||Old user (" << user_state.get_db_id() << ") - (" << user_state.get_tg_id() << ") " << user_state.get_language() << std::endl;
        return;
    }
    md_str.add(lang_handler.localize("start.welcome[BOLD]", "en"), tg::MDMode::BOLD);
    send_markdown(message->chat->id, md_str);
    md_str.clear();
    auto now = timetools::get_current_time();
    orm::User user_to_insert{-1, message->chat->id, message->chat->username, "en", "user", timetools::to_uint64(now)};
    int db_id{};
    try {
        db_id                          = db.get_storage().insert(user_to_insert);
        chat_states[message->chat->id] = UserChatState(message->chat->id, db_id);
    } catch (const std::system_error& e) {
        std::clog << "||Failed to insert user (" << user_to_insert.id << ") - "
            << user_to_insert.username << "(" << user_to_insert.telegram_id << ") " << user_to_insert.language
            << " /" << user_to_insert.created_at << std::endl;
        std::clog << "||" << e.what() << std::endl;
        md_str.add(lang_handler.localize("start.id-collision-error", user_to_insert.language));
        send_markdown(message->chat->id, md_str);
        return;
    }
    md_str.add(lang_handler.localize("start.registration-success", user_to_insert.language));
    send_markdown(message->chat->id, md_str);
    std::clog << "||New user (" << db_id << ") - "
        << user_to_insert.username << "(" << user_to_insert.telegram_id << ") " << user_to_insert.language
        << " /" << user_to_insert.created_at << std::endl;
}
void tg::TelegramHandler::change_language(tgb::Message::Ptr message) {
    auto user_id = get_user_by_tg_id(message->chat->id);
    tg::markdown_string md_str{};
    if (!user_id) {
        md_str.add(lang_handler.localize("universal.registartion-ask", "en"));
        send_markdown(message->chat->id, md_str);
        return;
    }
    auto user_state = get_chat_state(message->chat->id);
    md_str.add(lang_handler.localize("change-language.message", user_state.get_language()));
    tgb::InlineKeyboardMarkup::Ptr keyboard{new tgb::InlineKeyboardMarkup{}};
    for (auto lang_iter = lang_handler.get_dictionary().begin(); lang_iter != lang_handler.get_dictionary().end(); ++lang_iter) {
        std::vector<tgb::InlineKeyboardButton::Ptr> row{};
        tgb::InlineKeyboardButton::Ptr lang_button{new tgb::InlineKeyboardButton{}};
        lang_button->text = (user_state.get_language() == lang_iter->second.first.code ? "✓ " : "")
                                + lang_iter->second.first.name + " " + lang_iter->second.first.flag;
        lang_button->callbackData = std::string{"v1:lang-change:"} + lang_iter->second.first.code;
        row.push_back(lang_button);
        keyboard->inlineKeyboard.push_back(row);
    }
    send_markdown(message->chat->id, md_str, keyboard);
}
void tg::TelegramHandler::change_language_query(tgb::CallbackQuery::Ptr query, const std::string& language_code) {
    auto user_id = get_user_by_tg_id(query->message->chat->id);
    if (!user_id) { return; }
    auto user = db.get_user_by_id(*user_id);
    user.language = language_code;
    db.get_storage().update(user);
    get_chat_state(query->message->chat->id).set_language(language_code);
    //bot.getApi().answerCallbackQuery(query->id, lang_handler.localize("change-language.success[query]", language_code));
    bot.getApi().editMessageText(
        lang_handler.localize("change-language.choice", language_code),
        query->message->chat->id, query->message->messageId, "", "", nullptr, nullptr
    );
}
