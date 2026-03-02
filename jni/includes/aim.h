#pragma once
#include "uses.h"
#include "players.h"

namespace aimOffsets {
    inline int aimController = 0x80;
    inline int currentPitch = 0x18;
    inline int currentYaw = 0x1C;
    inline int smoothedPitch = 0x24;
    inline int smoothedYaw = 0x28;
    inline int weaponryController = 0x88;
    inline int weaponController = 0xA0;
    inline int weaponParameters = 0xA8;
    inline int fireState = 0x40;
    inline int currentWeaponId = 0x18;
}

constexpr int AIM_BONE_COUNT = 14;


struct AimbotSettings {
    bool enabled = false;
    bool drawFov = false;
    bool drawFovOutline = true;
    bool fovCheck = true;
    bool checkVisible = true;
    bool triggerBot = false;
    bool fireCheck = false;      
    bool knifeBot = false;       
    
    float fov = 100.0f;
    float smooth = 0.1f;
    float triggerDelay = 0.1f;
    float maxDistance = 500.0f;
    float targetSwitchDelay = 0.0f; 
    float killSwitchDelay = 0.0f;   
    
    
    
    
    
    bool selectedBones[AIM_BONE_COUNT] = { true, false, false, false, false, false, false, false, false, false, false, false, false, false };
    
    int hitChance = 100; 
    
    float fovColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};

inline AimbotSettings aimSettings;

class Aimbot {
public:
    
    void RenderFovCircle();
    
    
    void Update(const Vector3& targetPos, const Vector3& cameraPosition, 
                uint64_t player, uint64_t localPlayer);
    
    
    void Run(uint64_t player, uint64_t localPlayer, int localTeam, int boneIndex);
    
    
    void Render();
    
    
    Vector3 GetBonePosition(uint64_t player, int boneIndex);
    
    
    uint64_t GetBoneOffset(int boneIndex);
    
private:
    int lastWeapon = 0;
    float lastPitch = 0.0f;
    float lastYaw = 0.0f;
    
    
    bool isShooting = false;
    float lastShotTime = 0.0f;
};

inline Aimbot aimbot;
