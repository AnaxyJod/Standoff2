#pragma once
#include "uses.h"





namespace rcsOffsets {
    
    constexpr int gunParameters = 0x160;
    constexpr int recoilParameters = 0x158;
    
    
    constexpr int horizontalRange = 0x10;
    constexpr int verticalRange = 0x14;
    constexpr int progressFillingShotsCount = 0x60;
    
    
    constexpr int verticalRangeSafe = 0x64;
    constexpr int horizontalRangeSafe = 0x70;
    
    
    constexpr int progressFillingShotsCountSafe = 0xB8;
    
    
    constexpr int spreadSafe1 = 0x1E4;
    
    
    constexpr int recoilMultOnCrouch = 0x178;
    constexpr int recoilMultOnCrouchSafe = 0x1F8;
    
    
    constexpr int nullable_hasValue = 0x0;
    constexpr int nullable_key = 0x4;
    constexpr int nullable_value = 0x8;
}

struct RecoilSettings {
    bool enabled = false;       
    float horizontalValue = 0;  
    float verticalValue = 0;    
    bool noSpread = false;      
};

inline RecoilSettings recoilSettings;


struct OriginalRecoil {
    float horizontalRange = 0.0f;
    float verticalRange = 0.0f;
    float spread1 = 0.0f;
    bool saved = false;
    bool spreadSaved = false;
};

inline OriginalRecoil originalRecoil;


inline float ReadSafeFloat(uintptr_t address) {
    int key = mem.read<int>(address);
    int encodedValue = mem.read<int>(address + 0x4);
    int decoded = CalcValue(key, encodedValue);
    return *reinterpret_cast<float*>(&decoded);
}

inline void WriteSafeFloat(uintptr_t address, float value) {
    int key = mem.read<int>(address);
    int valueAsInt = *reinterpret_cast<int*>(&value);
    int encodedValue = CalcValue(key, valueAsInt);
    mem.write<int>(address + 0x4, encodedValue);
}

inline float ReadNullableSafeFloat(uintptr_t nullableAddress) {
    bool hasValue = mem.read<bool>(nullableAddress + rcsOffsets::nullable_hasValue);
    if (!hasValue) return 0.0f;
    
    int key = mem.read<int>(nullableAddress + rcsOffsets::nullable_key);
    int encodedValue = mem.read<int>(nullableAddress + rcsOffsets::nullable_value);
    int decoded = CalcValue(key, encodedValue);
    return *reinterpret_cast<float*>(&decoded);
}

inline void WriteNullableSafeFloat(uintptr_t nullableAddress, float value) {
    bool hasValue = mem.read<bool>(nullableAddress + rcsOffsets::nullable_hasValue);
    if (!hasValue) return;
    
    int key = mem.read<int>(nullableAddress + rcsOffsets::nullable_key);
    int valueAsInt = *reinterpret_cast<int*>(&value);
    int encodedValue = CalcValue(key, valueAsInt);
    mem.write<int>(nullableAddress + rcsOffsets::nullable_value, encodedValue);
}


inline int EncodeValue(int key, int value) {
    if ((key & 1) != 0) {
        
        return key ^ value;
    } else {
        
        
        
        int byte0 = (value >> 16) & 0xFF;  
        int byte1 = (value >> 8) & 0xFF;   
        int byte2 = value & 0xFF;          
        int byte3 = (value >> 24) & 0xFF;  
        return (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    }
}


inline int ReadNullableSafeInt(uintptr_t nullableAddress) {
    bool hasValue = mem.read<bool>(nullableAddress + rcsOffsets::nullable_hasValue);
    if (!hasValue) return 0;
    
    int key = mem.read<int>(nullableAddress + rcsOffsets::nullable_key);
    int encodedValue = mem.read<int>(nullableAddress + rcsOffsets::nullable_value);
    return CalcValue(key, encodedValue);
}

inline void WriteNullableSafeInt(uintptr_t nullableAddress, int value) {
    bool hasValue = mem.read<bool>(nullableAddress + rcsOffsets::nullable_hasValue);
    if (!hasValue) return;
    
    int key = mem.read<int>(nullableAddress + rcsOffsets::nullable_key);
    int encodedValue = EncodeValue(key, value);
    mem.write<int>(nullableAddress + rcsOffsets::nullable_value, encodedValue);
}

class RecoilControl {
public:
    void Render();
    void Reset();
private:
    bool wasEnabled = false;
    uint64_t lastWeaponController = 0;
    uint64_t cachedRecoilParams = 0;  
};

inline RecoilControl recoilControl;
