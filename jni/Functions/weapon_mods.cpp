#include "../includes/uses.h"
#include "../includes/players.h"
#include "../includes/rcs.h"
#include "../core/config.h"
#include "../core/game_data.h"

void InfinityAmmo() {
    if (!functions.infinityAmmo) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!Config::IsValidPtr(playerManager)) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!Config::IsValidPtr(localPlayer)) return;
    
    std::string currentWeapon = players.getWeaponName(localPlayer);
    if (!GameData::IsShootableWeapon(currentWeapon)) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!Config::IsValidPtr(weaponryController)) return;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!Config::IsValidPtr(weaponController)) return;
    
    mem.write<int32_t>(weaponController + 0x118, 0);
    mem.write<int32_t>(weaponController + 0x11C, 999);
    mem.write<int32_t>(weaponController + 0x120, 0);
    mem.write<int32_t>(weaponController + 0x124, 999);
}

void FireRateHack() {
    if (!functions.fireRate) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!Config::IsValidPtr(playerManager)) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!Config::IsValidPtr(localPlayer)) return;
    
    std::string currentWeapon = players.getWeaponName(localPlayer);
    if (!GameData::IsShootableWeapon(currentWeapon)) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!Config::IsValidPtr(weaponryController)) return;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!Config::IsValidPtr(weaponController)) return;
    
    mem.write<int32_t>(weaponController + 0x100, 0);
    mem.write<int32_t>(weaponController + 0x104, 0);
}

void Wallshot() {
    if (!functions.wallshot) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!Config::IsValidPtr(playerManager)) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!Config::IsValidPtr(localPlayer)) return;
    
    std::string currentWeapon = players.getWeaponName(localPlayer);
    if (GameData::IsKnife(currentWeapon)) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!Config::IsValidPtr(weaponryController)) return;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!Config::IsValidPtr(weaponController)) return;
    
    uint64_t gunParameters = mem.read<uint64_t>(weaponController + offsets::weaponParameters);
    if (!Config::IsValidPtr(gunParameters)) return;
    
    mem.write<float>(gunParameters + 0x148, 99999.0f);
    mem.write<float>(gunParameters + 0x1A0, 100.0f);
    mem.write<int32_t>(gunParameters + 0x1A4, 9999);
}

void NoRecoil() {
    if (!functions.noRecoil) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!Config::IsValidPtr(playerManager)) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!Config::IsValidPtr(localPlayer)) return;
    
    int health = players.getHealth(localPlayer);
    if (health <= 0) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!Config::IsValidPtr(weaponryController)) return;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!Config::IsValidPtr(weaponController)) return;
    
    uint64_t gunParams = mem.read<uint64_t>(weaponController + rcsOffsets::gunParameters);
    if (!Config::IsValidPtr(gunParams)) return;
    
    uint64_t recoilParams = mem.read<uint64_t>(gunParams + rcsOffsets::recoilParameters);
    if (!Config::IsValidPtr(recoilParams)) return;
    
    float noRecoilValue = 0.0f;
    mem.write<float>(recoilParams + rcsOffsets::horizontalRange, noRecoilValue);
    WriteNullableSafeFloat(recoilParams + rcsOffsets::horizontalRangeSafe, noRecoilValue);
    mem.write<float>(recoilParams + rcsOffsets::verticalRange, noRecoilValue);
    WriteNullableSafeFloat(recoilParams + rcsOffsets::verticalRangeSafe, noRecoilValue);
}
