#ifndef SM2_BOT_SRC_TOOLS_STRTOOLS_HPP
#define SM2_BOT_SRC_TOOLS_STRTOOLS_HPP
#pragma once

#include <string_view>
#include <sstream>
#include <string>
#include <vector>

inline std::string operator*(const std::string& str, size_t n) {
    std::string result{};
    result.reserve(str.size() * n);
    for (size_t i{}; i < n; ++i) result += str;
    return result;
}

namespace strtools {
    inline std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> output{};
        std::string part{};
        std::istringstream str_stream{str};
        while (std::getline(str_stream, part, delimiter)) {
            output.push_back(part);
        }
        return output;
    }
    inline std::string single_split(const std::string& str, char delimiter) {
        size_t pos = str.find(delimiter);
        if (pos == std::string::npos) return "";
        return str.substr(0, pos);
    }
    inline std::string merge(const std::vector<std::string>& strs, const std::string& delimiter) {
        std::string output{};
        for (size_t i = 0; i < strs.size(); ++i) {
            output += strs[i];
            if (i < strs.size() - 1) output += delimiter;
        }
        return output;
    }
    inline size_t utf8_strlen(const std::string_view str) {
        size_t count = 0;
        for (unsigned char c : str) {
            if ((c & 0xC0) != 0x80) ++count;
        }
        return count;
    }
}

#endif // SM2_BOT_SRC_TOOLS_STRTOOLS_HPP
