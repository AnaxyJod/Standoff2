#include "../includes/uses.h"
#include "../includes/players.h"
#include "../core/config.h"

void MoneyHack() {
    if (!functions.moneyHack) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!Config::IsValidPtr(playerManager)) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!Config::IsValidPtr(localPlayer)) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!Config::IsValidPtr(weaponryController)) return;
    
    uint64_t weaponManager = mem.read<uint64_t>(weaponryController + 0xB8);
    if (!Config::IsValidPtr(weaponManager)) return;
    
    int newPrice = 5;
    
    uint64_t weaponCache = mem.read<uint64_t>(weaponManager + 0x60);
    if (!Config::IsValidPtr(weaponCache)) return;
    
    uint64_t weaponParamsArray = mem.read<uint64_t>(weaponCache + 0x50);
    if (!Config::IsValidPtr(weaponParamsArray)) return;
    
    int arraySize = mem.read<int>(weaponParamsArray + 0x18);
    if (arraySize <= 0 || arraySize > Config::MAX_WEAPONS) return;
    
    for (int i = 0; i < arraySize; i++) {
        uint64_t weaponParams = mem.read<uint64_t>(weaponParamsArray + 0x20 + i * 0x8);
        if (!Config::IsValidPtr(weaponParams)) continue;
        
        mem.write<int32_t>(weaponParams + 0x48, 1);
        mem.write<int32_t>(weaponParams + 0x4C, 0);
        mem.write<int32_t>(weaponParams + 0x50, newPrice);
    }
}
