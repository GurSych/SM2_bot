#ifndef SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
#define SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
#pragma once

#include <unordered_map>
#include <string_view>
#include <optional>
#include <cstdint>
#include <string>
#include <tgbot/tgbot.h>
namespace tgb = TgBot;

#include "../db/DatabaseHandler.hpp"
#include "LanguageHandler.hpp"

namespace tg {
    using chat_id_t = std::int64_t;
    enum class ChatState {
        idle,
        add_word__word, add_word__definition
    };
    class UserChatState {
    public:
        UserChatState() = default;
        UserChatState(chat_id_t tg_id, uint64_t db_id, std::string lang = "en")
            : tg_id_{tg_id}, db_id_{db_id}, language_{lang}, state_{ChatState::idle} {}

        auto get_tg_id()    const { return tg_id_;    }
        auto get_db_id()    const { return db_id_;    }
        auto get_state()    const { return state_;    }
        auto get_language() const { return language_; }


        void set_tg_id(chat_id_t id)               { tg_id_ = id;      }
        void set_db_id(uint64_t id)                { db_id_ = id;      }
        void set_state(ChatState state)            { state_ = state;   }
        void set_language(const std::string& lang) { language_ = lang; }

    private:
        chat_id_t        tg_id_{-1};
        uint64_t           db_id_{};
        std::string language_{"en"};
        ChatState state_{ChatState::idle};
    };
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

        std::optional<uint64_t> get_user_by_tg_id(chat_id_t chat_id);
        UserChatState&          get_chat_state(chat_id_t chat_id);

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
        std::unordered_map<chat_id_t, UserChatState> chat_states{};
    };
}

#endif // SM2_BOT_SRC_TG_TELEGRAMHANDLER_HPP
