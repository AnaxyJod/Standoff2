#include "../includes/rcs.h"
#include "../includes/players.h"
#include "../includes/memory/helper.h"

static inline bool isValidPtr(uint64_t ptr) {
    return ptr != 0 && ptr >= 0x10000 && ptr <= 0x7fffffffffff;
}

void RecoilControl::Reset() {
    originalRecoil.saved = false;
    originalRecoil.spreadSaved = false;
    wasEnabled = false;
    lastWeaponController = 0;
    cachedRecoilParams = 0;
}

void RecoilControl::Render() {
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!isValidPtr(playerManager)) {
        Reset();
        return;
    }
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!isValidPtr(localPlayer)) {
        Reset();
        return;
    }
    
    int health = players.getHealth(localPlayer);
    if (health <= 0) {
        Reset();
        return;
    }
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!isValidPtr(weaponryController)) {
        Reset();
        return;
    }
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!isValidPtr(weaponController)) {
        Reset();
        return;
    }
    
    uint64_t gunParams = mem.read<uint64_t>(weaponController + rcsOffsets::gunParameters);
    if (!isValidPtr(gunParams)) {
        Reset();
        return;
    }
    
    uint64_t recoilParams = mem.read<uint64_t>(gunParams + rcsOffsets::recoilParameters);
    if (!isValidPtr(recoilParams)) {
        Reset();
        return;
    }
    
    
    
    
    if (recoilSettings.enabled) {
        float newH = recoilSettings.horizontalValue;
        float newV = recoilSettings.verticalValue;
        
        mem.write<float>(recoilParams + rcsOffsets::horizontalRange, newH);
        WriteNullableSafeFloat(recoilParams + rcsOffsets::horizontalRangeSafe, newH);
        mem.write<float>(recoilParams + rcsOffsets::verticalRange, newV);
        WriteNullableSafeFloat(recoilParams + rcsOffsets::verticalRangeSafe, newV);
    }

    
    static bool wasNoSpread = false;
    
    if (recoilSettings.noSpread && !originalRecoil.spreadSaved) {
        originalRecoil.spread1 = ReadSafeFloat(weaponController + rcsOffsets::spreadSafe1);
        originalRecoil.spreadSaved = true;
    }
    
    if (!recoilSettings.noSpread && wasNoSpread && originalRecoil.spreadSaved) {
        WriteSafeFloat(weaponController + rcsOffsets::spreadSafe1, originalRecoil.spread1);
    }
    
    wasNoSpread = recoilSettings.noSpread;
    
    if (recoilSettings.noSpread) {
        WriteSafeFloat(weaponController + rcsOffsets::spreadSafe1, 0.0f);
    }
}
