#pragma once
#include <cstdint>
#include "../core/config.h"

namespace Validation {
    inline bool IsPlayerValid(uint64_t player) {
        return Config::IsValidPtr(player);
    }
    
    inline bool IsHealthValid(int health) {
        return health > 0 && health <= 200;
    }
    
    inline bool IsTeamValid(int team) {
        return team >= 0 && team <= 3;
    }
    
    inline bool IsCountValid(int count, int maxCount) {
        return count > 0 && count <= maxCount;
    }
}
