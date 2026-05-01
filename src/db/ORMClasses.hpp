#ifndef SM2_BOT_SRC_DB_ORMCLASES_HPP
#define SM2_BOT_SRC_DB_ORMCLASES_HPP
#pragma once

#include <cstdint>
#include <string>

namespace orm {
    struct User {
        int               id{};
        int64_t  telegram_id{};
        std::string username{};
        std::string language{};
        std::string   status{};
        uint64_t  created_at{};
    };
}

#endif // SM2_BOT_SRC_DB_ORMCLASES_HPP
