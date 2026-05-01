#ifndef SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
#define SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
#pragma once

#include <type_traits>
#include <string_view>
#include <cstdint>
#include <string>
#include <tgbot/tgbot.h>
namespace tgb = TgBot;

#include "../db/DatabaseHandler.hpp"
#include "LanguageHandler.hpp"

namespace tg {
    using chat_id_t = std::int64_t;
    enum class MDMode {
        PLAIN, BOLD, ITALIC, UNDERLINE, STRIKETHROUGH, SPOILER, CODE, TEXT
    };
    class markdown_string {
    public:
        markdown_string()  = default;
        markdown_string(const std::string& str) : text{markdown_escape(str)} {}
        ~markdown_string() = default;

        template<typename... MDs>
        requires (std::is_same_v<MDs, tg::MDMode> && ...)
        void add(const std::string_view str, MDs... modes);

        void operator+=(const std::string& str);

        void clear() { text.clear(); }

        static std::string markdown_escape(const std::string_view str);
        static std::string markdown_apply (const std::string_view str, MDMode mode);

        std::string&       get_text()       { return text; }
        const std::string& get_text() const { return text; }

    private:
        std::string text{};
    };
    class TelegramHandler {
    public:
        TelegramHandler(std::string token, db::DatabaseHandler& db, tg::LanguageHandler& lang_handler);
        ~TelegramHandler() = default;

        tgb::Bot& getBot() { return bot; }

        void run_polling();

        void send_message (chat_id_t chat_id, const markdown_string& text);
        void send_markdown(chat_id_t chat_id, const markdown_string& text);
        void send_markdown(chat_id_t chat_id, const markdown_string& text, const tgb::InlineKeyboardMarkup::Ptr& keyboard);

        void send_message (chat_id_t chat_id, const std::string& text);
        void send_markdown(chat_id_t chat_id, const std::string& text);

        void start_command  (tgb::Message::Ptr message);
        void change_language(tgb::Message::Ptr message);

        void change_language_query(tgb::CallbackQuery::Ptr query, const std::string& language_code);

    private:
        tgb::Bot bot;
        db::DatabaseHandler& db;
        tg::LanguageHandler& lang_handler;
    };
}

#endif // SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
