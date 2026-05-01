#ifndef SM2_BOT_SRC_TG_LANGUAGEHANDLER_HPP
#define SM2_BOT_SRC_TG_LANGUAGEHANDLER_HPP
#pragma once

#include <unordered_map>
#include <string_view>
#include <string>

namespace tg {
    class LanguageHandler {
    public:
        struct LanguageData {
            std::string   name{};
            std::string   code{};
            std::string   flag{};
            std::string status{};
        };

    using language_map_t  = std::unordered_map<std::string, std::string>;
    using language_pair_t = std::pair<LanguageData, language_map_t>;
    using language_dict_t = std::unordered_map<std::string, language_pair_t>;

        LanguageHandler()  = default;
        ~LanguageHandler() = default;

        void load_dictionary(const std::string& path);

        std::string localize(const std::string& key, const std::string& language);

        language_dict_t& get_dictionary() { return dictionary; }

    private:
        language_dict_t dictionary{};
    };
}

#endif // SM2_BOT_SRC_TG_LANGUAGEHANDLER_HPP
