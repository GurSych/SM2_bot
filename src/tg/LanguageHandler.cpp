#include "LanguageHandler.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void tg::LanguageHandler::load_dictionary(const std::string& path) {
    std::ifstream file{path};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open dictionary file");
    }
    json data = json::parse(file);
    try {
        tg::LanguageHandler::LanguageData language_data{};
        data.at("data").at("language-code").get_to(language_data.code);
        data.at("data").at("language-name").get_to(language_data.name);
        data.at("data").at("language-flag").get_to(language_data.flag);
        data.at("data").at("language-status").get_to(language_data.status);
        dictionary[language_data.code] = {language_data,{}};
        auto& [_, dict] = dictionary[language_data.code];
        for (const auto& [key, value] : data.at("dictionary").items()) {
            dict[key] = value.get<std::string>();
        }
    } catch (const json::exception& e) {
        throw std::runtime_error("[json] Failed to parse dictionary file: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw std::runtime_error("[std] Failed to parse dictionary file: " + std::string(e.what()));
    }
    file.close();
}

std::string tg::LanguageHandler::localize(const std::string& key, const std::string& language) {
    if (auto lang_dict_it = dictionary.find(language); lang_dict_it != dictionary.end()) {
        if (auto key_pair_it = lang_dict_it->second.second.find(key); key_pair_it != lang_dict_it->second.second.end()) {
            return key_pair_it->second;
        }
    }
    return std::string(key);
}
