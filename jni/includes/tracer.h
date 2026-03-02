#pragma once
#include "uses.h"
#include "imgui/imgui/imgui.h"
#include <vector>
#include <chrono>
#include <mutex>
#include <cmath>


struct BulletTrace {
    Vector3 startPos;       
    Vector3 hitPos;         
    Vector3 enemyHitPos;    
    std::chrono::steady_clock::time_point timestamp;
    float duration;         
    bool isActive;
    bool hitEnemy;          
};

class BulletTracer {
public:
    
    bool enabled = false;
    float displayTime = 1.5f;           
    float fadeTime = 0.5f;              
    float lineThickness = 2.0f;         
    bool stopOnHit = false;             
    
    
    bool useGradient = false;           
    float color1[4] = { 1.0f, 0.3f, 0.3f, 1.0f };  
    float color2[4] = { 1.0f, 1.0f, 0.3f, 1.0f };  
    
    
    bool showHitPoint = false;          
    float hitPointRadius = 4.0f;        
    float hitPointColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; 
    
    int maxTraces = 10;                 
    
    
    void AddTrace(const Vector3& start, const Vector3& hit);
    
    
    void UpdateLastTraceWithEnemyHit(const Vector3& enemyHitPos);
    
    
    void Update();
    
    
    void Render(const Matrix& viewMatrix);
    
    
    void Clear();
    
    
    int GetActiveCount() const { return static_cast<int>(traces.size()); }
    
private:
    std::vector<BulletTrace> traces;
    std::mutex tracesMutex;
    
    void RemoveExpiredTraces();
};


inline BulletTracer bulletTracer;


namespace TracerOffsets {
    
    inline int raycastResult = 0x10;
    
    
    inline int resultHitList = 0x20;      
    inline int resultStartPos = 0x28;     
    inline int resultDirection = 0x34;    
    
    
    inline int hitDataPoint = 0x10;       
    
    
    inline int listItems = 0x10;
    inline int listCount = 0x18;
    
    
    inline int arrayFirstElement = 0x20;
}


inline Vector3 GetLocalPlayerPosition(uint64_t localPlayer) {
    if (!localPlayer) return Vector3::Zero();
    
    uint64_t ptr1 = mem.read<uint64_t>(localPlayer + offsets::player_ptr1);
    if (!ptr1) return Vector3::Zero();
    
    uint64_t ptr2 = mem.read<uint64_t>(ptr1 + offsets::player_ptr2);
    if (!ptr2) return Vector3::Zero();
    
    return mem.read<Vector3>(ptr2 + offsets::player_ptr3);
}


struct LastHitData {
    Vector3 startPos;
    Vector3 hitPoint;
    int lastShotCount = -1;
    uint64_t lastWeaponController = 0;  
    bool isValid = false;
};

inline LastHitData lastHitData;


inline int GetShotCounter(uint64_t localPlayer, uint64_t* outWeaponController = nullptr) {
    if (!localPlayer) return -1;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!weaponryController) return -1;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!weaponController) return -1;
    
    if (outWeaponController) {
        *outWeaponController = weaponController;
    }
    
    
    int val_110 = mem.read<int>(weaponController + 0x110);
    int val_118 = mem.read<int>(weaponController + 0x118);
    int val_A0 = mem.read<int>(weaponController + 0xA0);
    
    return val_110 + val_118 + val_A0;
}


inline bool GetRealHitPoint(uint64_t localPlayer, Vector3& outHitPoint, Vector3& outStartPos) {
    if (!localPlayer) return false;
    
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!weaponryController) return false;
    
    uint64_t gunController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!gunController) return false;
    
    
    uint64_t raycastHelper = mem.read<uint64_t>(gunController + 0xF8);
    if (!raycastHelper) return false;
    
    
    uint64_t raycastResult = mem.read<uint64_t>(raycastHelper + TracerOffsets::raycastResult);
    if (!raycastResult) return false;
    
    
    outStartPos = mem.read<Vector3>(raycastResult + TracerOffsets::resultStartPos);
    
    
    uint64_t hitList = mem.read<uint64_t>(raycastResult + TracerOffsets::resultHitList);
    if (!hitList) return false;
    
    int hitCount = mem.read<int>(hitList + TracerOffsets::listCount);
    if (hitCount <= 0) {
        
        Vector3 direction = mem.read<Vector3>(raycastResult + TracerOffsets::resultDirection);
        if (direction == Vector3::Zero() || outStartPos == Vector3::Zero()) return false;
        
        float maxDist = 500.0f;
        outHitPoint = Vector3(
            outStartPos.x + direction.x * maxDist,
            outStartPos.y + direction.y * maxDist,
            outStartPos.z + direction.z * maxDist
        );
        return true;
    }
    
    
    uint64_t hitArray = mem.read<uint64_t>(hitList + TracerOffsets::listItems);
    if (!hitArray) return false;
    
    
    uint64_t firstHit = mem.read<uint64_t>(hitArray + TracerOffsets::arrayFirstElement);
    if (!firstHit) return false;
    
    
    outHitPoint = mem.read<Vector3>(firstHit + TracerOffsets::hitDataPoint);
    
    return true;
}


inline bool CheckNewShot(uint64_t localPlayer, Vector3& outStartPos, Vector3& outHitPoint) {
    uint64_t currentWeaponController = 0;
    int currentShotCount = GetShotCounter(localPlayer, &currentWeaponController);
    
    
    if (currentWeaponController != lastHitData.lastWeaponController) {
        lastHitData.lastWeaponController = currentWeaponController;
        lastHitData.lastShotCount = currentShotCount;
        return false;  
    }
    
    
    
    bool shotFired = (lastHitData.lastShotCount >= 0 && 
                      currentShotCount > lastHitData.lastShotCount &&
                      currentShotCount - lastHitData.lastShotCount < 100);  
    
    
    if (currentShotCount >= 0) {
        lastHitData.lastShotCount = currentShotCount;
    }
    
    if (!shotFired) {
        return false;
    }
    
    
    Vector3 currentStart, currentHit;
    if (GetRealHitPoint(localPlayer, currentHit, currentStart)) {
        if (currentStart != Vector3::Zero() && currentHit != Vector3::Zero()) {
            outStartPos = currentStart;
            outHitPoint = currentHit;
            return true;
        }
    }
    
    
    Vector3 playerPos = GetLocalPlayerPosition(localPlayer);
    if (playerPos == Vector3::Zero()) return false;
    
    outStartPos = playerPos;
    outStartPos.y += 1.6f; 
    
    
    return false;
}
