#pragma once
#include <string>
#include <vector>

namespace GameData {
    const std::vector<std::string> KNIFE_WEAPONS = {
        "Knife", "Karambit", "Butterfly", "M9 Bayonet",
        "Kunai", "jKommando", "Scorpion", "Flip Knife",
        "Tanto", "Dual Daggers", "Kukri", "Stiletto",
        "Fang", "Sting"
    };
    
    const std::vector<std::string> BOMB_WEAPONS = {
        "Bomb", "C4"
    };
    
    const std::vector<std::string> GRENADE_WEAPONS = {
        "HE Grenade", "Smoke", "Flash", 
        "Molotov", "Incendiary", "Snowball"
    };
    
    inline bool IsKnife(const std::string& weapon) {
        for (const auto& knife : KNIFE_WEAPONS) {
            if (weapon == knife) return true;
        }
        return false;
    }
    
    inline bool IsBomb(const std::string& weapon) {
        for (const auto& bomb : BOMB_WEAPONS) {
            if (weapon == bomb) return true;
        }
        return false;
    }
    
    inline bool IsGrenade(const std::string& weapon) {
        for (const auto& grenade : GRENADE_WEAPONS) {
            if (weapon == grenade) return true;
        }
        return false;
    }
    
    inline bool IsShootableWeapon(const std::string& weapon) {
        return !IsKnife(weapon) && !IsBomb(weapon) && !IsGrenade(weapon);
    }
    
    inline int GetHeadshotThreshold(const std::string& weapon) {
        if (weapon == "AWM") return 200;
        if (weapon == "Knife") return 170;
        return 101;
    }
}
