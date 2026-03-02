#include "../includes/aim.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/imgui/draw/draw.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>


static std::chrono::steady_clock::time_point g_lastAimWrite;
static constexpr int AIM_WRITE_INTERVAL_MS = 16; 


struct TargetTrackingData {
    uint64_t currentTarget = 0;
    int lastTargetHealth = 0;
    std::chrono::steady_clock::time_point targetSwitchTime;
    std::chrono::steady_clock::time_point killTime;
    bool waitingAfterKill = false;
};
inline TargetTrackingData targetTracking;


struct FireCheckData {
    bool hitChanceResult = true; 
    int lastShotCountForHitChance = -1; 
    int lastShotCount = -1; 
    uint64_t lastWeaponController = 0;
    std::chrono::steady_clock::time_point lastFireTime;
    bool wasFiring = false;
};
inline FireCheckData fireCheckData;


inline std::mt19937& GetRNG() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}



inline bool IsPlayerFiring(uint64_t localPlayer) {
    if (!localPlayer) return false;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!weaponryController) {
        weaponryController = mem.read<uint64_t>(localPlayer + 0x68);
    }
    if (!weaponryController) return false;
    
    uint64_t weaponController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!weaponController) return false;
    
    
    if (weaponController != fireCheckData.lastWeaponController) {
        fireCheckData.lastWeaponController = weaponController;
        fireCheckData.lastShotCount = -1;
        fireCheckData.wasFiring = false;
    }
    
    bool isFiring = false;
    
    
    
    uint8_t fireState = mem.read<uint8_t>(weaponController + 0x140);
    if (fireState == 3) {
        isFiring = true;
    }
    
    
    
    int shotCount = mem.read<int>(weaponController + 0x110);
    if (fireCheckData.lastShotCount >= 0 && shotCount > fireCheckData.lastShotCount) {
        isFiring = true;
        fireCheckData.lastFireTime = std::chrono::steady_clock::now();
        fireCheckData.wasFiring = true;
    }
    fireCheckData.lastShotCount = shotCount;
    
    
    
    if (!isFiring && fireCheckData.wasFiring) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - fireCheckData.lastFireTime).count();
        if (elapsed < 200) {
            isFiring = true;
        } else {
            fireCheckData.wasFiring = false;
        }
    }
    
    return isFiring;
}

void Aimbot::RenderFovCircle() {
    if (!aimSettings.drawFov) return;
    
    auto displayInfo = draw::getDisplayInfo();
    ImVec2 center(displayInfo.width / 2.0f, displayInfo.height / 2.0f);
    ImU32 circleColor = ImGui::ColorConvertFloat4ToU32(
        ImVec4(aimSettings.fovColor[0], aimSettings.fovColor[1], 
               aimSettings.fovColor[2], aimSettings.fovColor[3]));
    ImU32 outlineColor = IM_COL32(0, 0, 0, 150);
    
    float half_width = 2.0f / 2.0f; 
    
    if (aimSettings.drawFovOutline) {
        
        ImGui::GetBackgroundDrawList()->AddCircle(center, aimSettings.fov + half_width, outlineColor, 64, 2.0f);
    }
    
    ImGui::GetBackgroundDrawList()->AddCircle(center, aimSettings.fov, circleColor, 64, 2.0f);
    if (aimSettings.drawFovOutline) {
        
        ImGui::GetBackgroundDrawList()->AddCircle(center, aimSettings.fov - half_width, outlineColor, 64, 2.0f);
    }
}

uint64_t Aimbot::GetBoneOffset(int boneIndex) {
    switch (boneIndex) {
        case 0: return offsets::bone_head;
        case 1: return offsets::bone_neck;
        case 2: return offsets::bone_spine;
        case 3: return offsets::bone_hip;
        case 4: return offsets::bone_leftShoulder;
        case 5: return offsets::bone_leftUpperArm;
        case 6: return offsets::bone_leftHand;
        case 7: return offsets::bone_rightShoulder;
        case 8: return offsets::bone_rightUpperArm;
        case 9: return offsets::bone_rightHand;
        case 10: return offsets::bone_leftUpperLeg;
        case 11: return offsets::bone_leftLowerLeg;
        case 12: return offsets::bone_rightUpperLeg;
        case 13: return offsets::bone_rightLowerLeg;
        default: return offsets::bone_head;
    }
}

Vector3 Aimbot::GetBonePosition(uint64_t player, int boneIndex) {
    uint64_t boneOffset = GetBoneOffset(boneIndex);
    
    
    uint64_t characterView = mem.read<uint64_t>(player + offsets::playerCharacterView);
    if (!characterView) return Vector3::Zero();
    
    
    static int updateCounter = 0;
    if (updateCounter++ % 2 == 0) { 
        mem.write<bool>(characterView + offsets::viewVisible, true);
    }
    
    
    uint64_t bipedMap = mem.read<uint64_t>(characterView + offsets::bipedMap);
    if (!bipedMap) return Vector3::Zero();
    
    
    uint64_t boneTransform = mem.read<uint64_t>(bipedMap + boneOffset);
    if (!boneTransform) return Vector3::Zero();
    
    
    return GameString::GetPosition(boneTransform);
}

void Aimbot::Update(const Vector3& targetPos, const Vector3& playerPosition,
                    uint64_t player, uint64_t localPlayer) {
    
    uint64_t aimController = mem.read<uint64_t>(localPlayer + 0x80);
    if (!aimController) {
        aimController = mem.read<uint64_t>(localPlayer + 0x60);
    }
    if (!aimController) return;
    
    
    uint64_t aimingData = mem.read<uint64_t>(aimController + aimOffsets::aimData);
    if (!aimingData) return;
    
    
    uint64_t camTransform = mem.read<uint64_t>(aimController + 0x80);
    Vector3 cameraPos = playerPosition; 
    if (camTransform) {
        cameraPos = GameString::GetPosition(camTransform);
    }
    
    
    float currentPitch = mem.read<float>(aimingData + 0x18);
    float currentYaw = mem.read<float>(aimingData + 0x1C);
    
    
    Vector3 direction = targetPos - cameraPos;
    float distance = direction.length();
    if (distance <= 0.0001f) return;
    
    
    float targetPitch = -asin(direction.y / distance) * (180.0f / M_PI);
    float targetYaw = atan2(direction.x, direction.z) * (180.0f / M_PI);
    
    
    float pitchDelta = targetPitch - currentPitch;
    float yawDelta = targetYaw - currentYaw;
    
    
    while (yawDelta > 180.0f) yawDelta -= 360.0f;
    while (yawDelta < -180.0f) yawDelta += 360.0f;
    
    
    float smoothFactor = 1.0f;
    if (aimSettings.smooth > 0.1f) {
        smoothFactor = 1.0f / (1.0f + aimSettings.smooth * 0.5f);
        smoothFactor = std::clamp(smoothFactor, 0.03f, 1.0f);
    }
    
    
    float newPitch = currentPitch + pitchDelta * smoothFactor;
    float newYaw = currentYaw + yawDelta * smoothFactor;
    newPitch = std::clamp(newPitch, -89.0f, 89.0f);
    
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastAimWrite).count();
    if (elapsed < AIM_WRITE_INTERVAL_MS) {
        return; 
    }
    g_lastAimWrite = now;
    
    
    mem.write<float>(aimingData + 0x18, newPitch);
    mem.write<float>(aimingData + 0x1C, newYaw);
    mem.write<float>(aimingData + 0x24, newPitch);
    mem.write<float>(aimingData + 0x28, newYaw);
}

void Aimbot::Run(uint64_t player, uint64_t localPlayer, int localTeam, int boneIndex) {
    if (!aimSettings.enabled) return;
    
    
    if (aimSettings.fireCheck && !IsPlayerFiring(localPlayer)) {
        return;
    }
    
    auto displayInfo = draw::getDisplayInfo();
    ImVec2 screenCenter(displayInfo.width / 2.0f, displayInfo.height / 2.0f);
    
    
    
    
    if (aimSettings.checkVisible) {
        uint64_t occlusionController = mem.read<uint64_t>(player + 0xB0);
        if (occlusionController) {
            int visibilityState = mem.read<int>(occlusionController + 0x34);
            int occlusionState = mem.read<int>(occlusionController + 0x38);
            
            if (visibilityState != 2 || occlusionState == 1) return;
        }
    }
    
    
    Matrix freshViewMatrix = mem.read<Matrix>(
        mem.read<uint64_t>(
            mem.read<uint64_t>(
                mem.read<uint64_t>(localPlayer + offsets::viewMatrix_ptr1) + 
                offsets::viewMatrix_ptr2) + 
            offsets::viewMatrix_ptr3) + 
        offsets::viewMatrix_ptr4);
    
    
    Vector3 localPos = getPlayerPosition(localPlayer);
    
    
    Vector3 enemyPos = GetBonePosition(player, boneIndex);
    if (enemyPos == Vector3::Zero()) return;
    
    
    bool isOnScreen = false;
    ImVec2 screenPos = worldToScreen(enemyPos, freshViewMatrix, &isOnScreen);
    
    if (!isOnScreen) return;
    
    
    float dx = screenPos.x - screenCenter.x;
    float dy = screenPos.y - screenCenter.y;
    float dist2D = std::sqrt(dx * dx + dy * dy);
    float dist3D = (enemyPos - localPos).length();
    
    
    if (aimSettings.fovCheck) {
        if (dist2D > aimSettings.fov || dist3D > 500.0f) return;
    } else {
        if (dist2D > 700 || dist3D > 500.0f) return;
    }
    
    
    
    
    
    Update(enemyPos, localPos, player, localPlayer);
    
    
    if (aimSettings.triggerBot) {
        
        uint64_t occlusionController = mem.read<uint64_t>(player + 0xB0);
        if (occlusionController) {
            int visibilityState = mem.read<int>(occlusionController + 0x34);
            int occlusionState = mem.read<int>(occlusionController + 0x38);
            if (visibilityState != 2 || occlusionState == 1) return;
        }
        
        lastShotTime += ImGui::GetIO().DeltaTime;
        
        uint64_t weaponryController = mem.read<uint64_t>(localPlayer + 0x88);
        if (!weaponryController) {
            weaponryController = mem.read<uint64_t>(localPlayer + 0x68);
        }
        if (!weaponryController) return;
        
        uint64_t weaponController = mem.read<uint64_t>(weaponryController + 0xA0);
        if (!weaponController) return;
        
        if (!isShooting) {
            if (lastShotTime >= aimSettings.triggerDelay) {
                mem.write<uint8_t>(weaponController + 0x140, 3);
                isShooting = true;
                lastShotTime = 0.0f;
            }
        } else {
            if (lastShotTime >= aimSettings.triggerDelay) {
                mem.write<uint8_t>(weaponController + 0x140, 2);
                isShooting = false;
                lastShotTime = 0.0f;
            }
        }
    }
}

void Aimbot::Render() {
    
    RenderFovCircle();
    
    if (!aimSettings.enabled) return;
    
    
    bool anyBoneSelected = false;
    for (int i = 0; i < AIM_BONE_COUNT; i++) {
        if (aimSettings.selectedBones[i]) {
            anyBoneSelected = true;
            break;
        }
    }
    if (!anyBoneSelected) return;
    
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!playerManager) return;
    
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!localPlayer) return;
    
    
    if (!aimSettings.knifeBot) {
        std::string currentWeapon = players.getWeaponName(localPlayer);
        bool isKnife = (currentWeapon == "Knife" || currentWeapon == "Karambit" ||
                        currentWeapon == "Butterfly" || currentWeapon == "M9 Bayonet" ||
                        currentWeapon == "Kunai" || currentWeapon == "jKommando" ||
                        currentWeapon == "Scorpion" || currentWeapon == "Flip Knife" ||
                        currentWeapon == "Tanto" || currentWeapon == "Dual Daggers" ||
                        currentWeapon == "Kukri" || currentWeapon == "Stiletto" ||
                        currentWeapon == "Fang" || currentWeapon == "Sting");
        if (isKnife) return;
    }
    if (!localPlayer) return;
    
    
    uint64_t playersList = mem.read<uint64_t>(playerManager + offsets::entityList);
    if (!playersList) return;
    
    int playersCount = mem.read<int>(playersList + offsets::entityList_count);
    int localTeam = players.getTeam(localPlayer);
    
    
    Matrix freshViewMatrix = mem.read<Matrix>(
        mem.read<uint64_t>(
            mem.read<uint64_t>(
                mem.read<uint64_t>(localPlayer + offsets::viewMatrix_ptr1) + 
                offsets::viewMatrix_ptr2) + 
            offsets::viewMatrix_ptr3) + 
        offsets::viewMatrix_ptr4);
    
    
    float closestDist = FLT_MAX;
    uint64_t closestPlayer = 0;
    int closestBone = 0;
    
    auto displayInfo = draw::getDisplayInfo();
    ImVec2 screenCenter(displayInfo.width / 2.0f, displayInfo.height / 2.0f);
    
    Vector3 localPos = getPlayerPosition(localPlayer);
    
    for (int i = 0; i < playersCount; i++) {
        uint64_t player = mem.read<uint64_t>(
            mem.read<uint64_t>(playersList + offsets::player_ptr1) + 
            offsets::player_ptr2 + offsets::player_ptr3 * i);
        
        if (!player) continue;
        if (players.getTeam(player) == localTeam) continue;
        
        int health = players.getHealth(player);
        if (health <= 0) continue;
        
        
        for (int boneIdx = 0; boneIdx < AIM_BONE_COUNT; boneIdx++) {
            if (!aimSettings.selectedBones[boneIdx]) continue;
            
            
            Vector3 bonePos = GetBonePosition(player, boneIdx);
            if (bonePos == Vector3::Zero()) continue;
            
            
            bool isOnScreen = false;
            ImVec2 screenPos = worldToScreen(bonePos, freshViewMatrix, &isOnScreen);
            if (!isOnScreen) continue;
            
            
            float dx = screenPos.x - screenCenter.x;
            float dy = screenPos.y - screenCenter.y;
            float dist2D = std::sqrt(dx * dx + dy * dy);
            
            
            if (aimSettings.fovCheck) {
                if (dist2D > aimSettings.fov) continue;
            } else {
                
                if (dist2D > 700) continue;
            }
            
            
            float dist3D = (bonePos - localPos).length();
            if (dist3D > 500.0f) continue;
            
            
            if (dist2D < closestDist) {
                closestDist = dist2D;
                closestPlayer = player;
                closestBone = boneIdx;
            }
        }
    }
    
    
    if (closestPlayer) {
        auto now = std::chrono::steady_clock::now();
        
        
        if (aimSettings.killSwitchDelay > 0.0f && targetTracking.waitingAfterKill) {
            float elapsed = std::chrono::duration<float>(now - targetTracking.killTime).count();
            if (elapsed < aimSettings.killSwitchDelay) {
                return; 
            }
            targetTracking.waitingAfterKill = false;
        }
        
        
        if (closestPlayer != targetTracking.currentTarget) {
            
            if (targetTracking.currentTarget != 0 && targetTracking.lastTargetHealth > 0) {
                int prevHealth = players.getHealth(targetTracking.currentTarget);
                if (prevHealth <= 0 && aimSettings.killSwitchDelay > 0.0f) {
                    
                    targetTracking.killTime = now;
                    targetTracking.waitingAfterKill = true;
                    targetTracking.currentTarget = 0;
                    return;
                }
            }
            
            
            if (aimSettings.targetSwitchDelay > 0.0f && targetTracking.currentTarget != 0) {
                float elapsed = std::chrono::duration<float>(now - targetTracking.targetSwitchTime).count();
                if (elapsed < aimSettings.targetSwitchDelay) {
                    return; 
                }
            }
            
            
            targetTracking.currentTarget = closestPlayer;
            targetTracking.targetSwitchTime = now;
        }
        
        
        targetTracking.lastTargetHealth = players.getHealth(closestPlayer);
        
        Run(closestPlayer, localPlayer, localTeam, closestBone);
    } else {
        
        if (targetTracking.currentTarget != 0) {
            
            if (targetTracking.lastTargetHealth > 0) {
                int health = players.getHealth(targetTracking.currentTarget);
                if (health <= 0 && aimSettings.killSwitchDelay > 0.0f) {
                    targetTracking.killTime = std::chrono::steady_clock::now();
                    targetTracking.waitingAfterKill = true;
                }
            }
        }
        targetTracking.currentTarget = 0;
        targetTracking.lastTargetHealth = 0;
    }
}
