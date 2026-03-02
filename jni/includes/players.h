#pragma once
#include "uses.h"

class g_players{
public:
    int getHealth(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "health"); 
    }

    int getArmor(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "armor");
    }

    int getMoney(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "money");
    }

    int getPing(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "ping");
    }

    int getHelmet(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "helmet");
    }

    int getScore(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "score");
    }

    int getKills(uint64_t player) {
        return GameString::getPlayerProperty<int>(player, "kills");
    }

    std::string getName(uint64_t player) { 
        if (player == 0) return "";
        
        auto photon = GameString::getPhotonPointer(player);
        if (photon == 0 || photon < 0x10000 || photon > 0x7fffffffffff) return "";
        
        uint64_t namePtr = mem.read<uint64_t>(photon + offsets::name);
        if (namePtr == 0 || namePtr < 0x10000 || namePtr > 0x7fffffffffff) return "";
        
        GameString gs = mem.read<GameString>(namePtr);
        return gs.getString();
    }

    std::string getWeaponName(uint64_t player) { 
        if (player == 0) return "";
        
	    
	    uint64_t WeaponryController = mem.read<uint64_t>(player + offsets::weaponryController);
	    if(WeaponryController == 0 || WeaponryController < 0x10000 || WeaponryController > 0x7fffffffffff) return "";
	    
		
		uint64_t WeaponController = mem.read<uint64_t>(WeaponryController + offsets::currentWeaponController);
		if(WeaponController == 0 || WeaponController < 0x10000 || WeaponController > 0x7fffffffffff) return "";
		
		
		uint64_t WeaponParameters = mem.read<uint64_t>(WeaponController + offsets::weaponParameters);
		if(WeaponParameters == 0 || WeaponParameters < 0x10000 || WeaponParameters > 0x7fffffffffff) return "";
		
		
		uint64_t namePtr = mem.read<uint64_t>(WeaponParameters + offsets::weaponDisplayName);
		if (namePtr == 0 || namePtr < 0x10000 || namePtr > 0x7fffffffffff) return "";
		
		GameString gs = mem.read<GameString>(namePtr);
		return gs.getString();
    }

    
    std::string getLocalWeaponName(uint64_t player) { 
	    return getWeaponName(player);
    }

    uint8_t getTeam(uint64_t player) {
        return mem.read<uint8_t>(player + offsets::team);
    }

    void Update();
};

inline g_players players;