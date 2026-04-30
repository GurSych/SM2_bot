#include "TelegramHandler.hpp"

tg::TelegramHandler::TelegramHandler(std::string token) : bot{token} {
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
void tg::markdown_string::add(const std::string& str, MDs... modes) {
    if constexpr (sizeof...(modes) == 0)
        text += markdown_escape(str);
    else {
        std::string md_str{str};
        ((md_str = markdown_apply(md_str, modes)),...);
        text += md_str;
    }
}

std::string tg::markdown_string::markdown_escape(const std::string& text) {
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

std::string tg::markdown_string::markdown_apply(const std::string& str, MDMode mode)  {
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
    tg::markdown_string md_str{};
    md_str.add("Hello there!", tg::MDMode::BOLD);
    send_markdown(message->chat->id, md_str);
}
