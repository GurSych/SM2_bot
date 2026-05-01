#ifndef SM2_BOT_SRC_TOOLS_TIMETOOLS_HPP
#define SM2_BOT_SRC_TOOLS_TIMETOOLS_HPP
#pragma once

#include <cstdint>
#include <chrono>

namespace timetools {
    using timestamp_ms_t  = std::uint64_t;
    using time_point_ms_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
    inline timestamp_ms_t to_uint64(time_point_ms_t tp) {
        return static_cast<timestamp_ms_t>(tp.time_since_epoch().count());
    }
    inline time_point_ms_t from_uint64(timestamp_ms_t ts) {
        return time_point_ms_t{std::chrono::milliseconds{ts}};
    }
    inline time_point_ms_t get_current_time() {
        return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    }
}

#endif // SM2_BOT_SRC_TOOLS_TIMETOOLS_HPP
