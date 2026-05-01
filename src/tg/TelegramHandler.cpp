#include "TelegramHandler.hpp"

#include <chrono>
#include "../tools/TimeTools.hpp"
#include "../db/ORMClasses.hpp"

tg::TelegramHandler::TelegramHandler(std::string token, db::DatabaseHandler& db, tg::LanguageHandler& lang_handler)
    : bot{token}, db{db}, lang_handler{lang_handler} {
    bot.getEvents().onCommand("start", [this](tgb::Message::Ptr message) {
        start_command(message);
    });
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
    auto user = db.get_storage().get_all<orm::User>(sql::where(sql::c(&orm::User::telegram_id) == message->chat->id));
    tg::markdown_string md_str{};
    md_str.add(lang_handler.localize("start.welcome[BOLD]", (user.size() == 1 ? user[0].language : "en")), tg::MDMode::BOLD);
    send_markdown(message->chat->id, md_str);
    md_str.clear();
    if (user.size() == 1) {
        md_str.add(lang_handler.localize("start.registrated-id[1]", user[0].language));
        md_str.add(lang_handler.localize("start.registrated-id[2|ITALIC]", user[0].language), tg::MDMode::ITALIC);
        md_str.add(std::to_string(user[0].id), tg::MDMode::CODE);
        send_markdown(message->chat->id, md_str);
        std::clog << "||Old user (" << user[0].id << ") - "
            << user[0].username << "(" << user[0].telegram_id << ") " << user[0].language
            << " /" << user[0].created_at << std::endl;
        return;
    }
    auto now = timetools::get_current_time();
    orm::User user_to_insert{-1, message->chat->id, message->chat->username, "en", "user", timetools::to_uint64(now)};
    int db_id{};
    try {
        db_id = db.get_storage().insert(user_to_insert);
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
