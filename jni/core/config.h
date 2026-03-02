#pragma once
#include <cstdint>

namespace Config {
    constexpr uint64_t MIN_VALID_PTR = 0x10000;
    constexpr uint64_t MAX_VALID_PTR = 0x7fffffffffff;
    
    constexpr int AIM_WRITE_INTERVAL_MS = 16;
    constexpr float MAX_AIM_DISTANCE = 500.0f;
    constexpr float MAX_SCREEN_DISTANCE = 700.0f;
    
    constexpr int MAX_PLAYERS = 100;
    constexpr int MAX_WEAPONS = 200;
    
    inline bool IsValidPtr(uint64_t ptr) {
        return ptr != 0 && ptr >= MIN_VALID_PTR && ptr <= MAX_VALID_PTR;
    }
}
