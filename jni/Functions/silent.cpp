#include "../includes/silent.h"
#include "../includes/rcs.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/imgui/draw/draw.h"
#include <cmath>
#include <chrono>

static Vector2 GetCurrentAimAngles(uint64_t localPlayer) {
    uint64_t aimController = mem.read<uint64_t>(localPlayer + 0x80);
    if (!aimController) {
        aimController = mem.read<uint64_t>(localPlayer + 0x60);
    }
    if (!aimController) return Vector2(0, 0);
    
    uint64_t aimingData = mem.read<uint64_t>(aimController + 0x90);
    if (!aimingData) return Vector2(0, 0);
    
    float pitch = mem.read<float>(aimingData + 0x18);
    float yaw = mem.read<float>(aimingData + 0x1C);
    
    return Vector2(pitch, yaw);
}

static Vector3 GetCameraPosition(uint64_t localPlayer) {
    uint64_t aimController = mem.read<uint64_t>(localPlayer + 0x80);
    if (!aimController) {
        aimController = mem.read<uint64_t>(localPlayer + 0x60);
    }
    if (!aimController) {
        return getPlayerPosition(localPlayer);
    }
    
    uint64_t camTransform = mem.read<uint64_t>(aimController + 0x80);
    if (!camTransform) {
        return getPlayerPosition(localPlayer);
    }
    
    Vector3 pos = GameString::GetPosition(camTransform);
    return (pos == Vector3::Zero()) ? getPlayerPosition(localPlayer) : pos;
}

static float NormalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

void Silent::RenderMenu() {
    ImGui::Checkbox("Enable Silent", &silentSettings.enabled);
    if (silentSettings.enabled) {
        ImGui::SliderFloat("FOV", &silentSettings.fov, 50.0f, 500.0f);
        ImGui::SliderFloat("Max Distance", &silentSettings.maxDistance, 50.0f, 500.0f);
        ImGui::Checkbox("Auto Multiplier", &silentSettings.autoMultiplier);
        if (!silentSettings.autoMultiplier) {
            ImGui::SliderFloat("Multiplier", &silentSettings.multiplier, 0.5f, 5.0f);
        }
        ImGui::Checkbox("Disable Cam Deviation", &silentSettings.disableCamDev);
        ImGui::Checkbox("Show Debug", &silentSettings.showDebug);
        
        ImGui::Separator();
        ImGui::Text("No Recoil Options:");
        ImGui::Checkbox("RecoilParams", &silentSettings.noRecoilParams);
        ImGui::Checkbox("RecoilData", &silentSettings.noRecoilData);
        ImGui::Checkbox("AccuracyData", &silentSettings.noAccuracyData);
        ImGui::Checkbox("GunController Floats", &silentSettings.noGunControllerFloats);
        ImGui::Checkbox("GunParams RecoilControl", &silentSettings.modifyGunParams);
    }
}

void Silent::Render() {
    if (!silentSettings.enabled) return;
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!playerManager) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!localPlayer) return;
    
    int selfHealth = players.getHealth(localPlayer);
    if (selfHealth <= 0) return;
    
    uint64_t weaponryController = mem.read<uint64_t>(localPlayer + offsets::weaponryController);
    if (!weaponryController) return;
    
    uint64_t gunController = mem.read<uint64_t>(weaponryController + offsets::currentWeaponController);
    if (!gunController) return;
    
    
    
    bool skipNoRecoil = false;
    int gunState = -1;
    uint64_t gunStateSimple = mem.read<uint64_t>(gunController + 0x180);
    if (gunStateSimple) {
        gunState = mem.read<int>(gunStateSimple + 0x10);
        
        
        
        
    }
    
    
    uint64_t recoilData = mem.read<uint64_t>(gunController + 0x158);
    if (!recoilData) return;
    
    
    uint64_t gunParameters = mem.read<uint64_t>(gunController + 0x160);
    
    
    
    
    uint64_t recoilParamsFromData = mem.read<uint64_t>(recoilData + 0x48);
    uint64_t recoilParamsFromGun = gunParameters ? mem.read<uint64_t>(gunParameters + 0x158) : 0;
    
    
    uint64_t recoilParams = recoilParamsFromData ? recoilParamsFromData : recoilParamsFromGun;
    if (!recoilParams) return;
    
    std::string weaponName = players.getWeaponName(localPlayer);
    
    
    float camDevCoeff = mem.read<float>(recoilParams + 0x48);
    if (camDevCoeff < 0.001f) {
        camDevCoeff = mem.read<float>(recoilParams + 0x98);
    }
    
    float dynamicMultiplier = silentSettings.multiplier;
    if (silentSettings.autoMultiplier && camDevCoeff > 0.001f) {
        dynamicMultiplier = 1.0f / camDevCoeff;
    }
    
    
    if (silentSettings.disableCamDev && !skipNoRecoil) {
        mem.write<float>(recoilParams + 0x48, 0.0f);
        mem.write<int>(recoilParams + 0x94, 1);
        mem.write<float>(recoilParams + 0x98, 0.0f);
    }
    
    float zero_f = 0.0f;
    int zero_i = 0;
    int one_i = 1;
    
    
    
    if (silentSettings.noRecoilParams && !skipNoRecoil) {
        
        mem.write<float>(recoilParams + 0x10, zero_f);  
        mem.write<float>(recoilParams + 0x14, zero_f);  
        mem.write<float>(recoilParams + 0x38, zero_f);  
        mem.write<float>(recoilParams + 0x48, zero_f);  
        mem.write<float>(recoilParams + 0x4C, zero_f);  
        mem.write<float>(recoilParams + 0x50, zero_f);  
        mem.write<float>(recoilParams + 0x54, zero_f);  
        mem.write<int>(recoilParams + 0x60, zero_i);    
        
        
        mem.write<int>(recoilParams + 0x64, one_i);
        mem.write<float>(recoilParams + 0x68, zero_f);
        mem.write<int>(recoilParams + 0x70, one_i);
        mem.write<float>(recoilParams + 0x74, zero_f);
        mem.write<int>(recoilParams + 0x7C, one_i);
        mem.write<float>(recoilParams + 0x80, zero_f);
        mem.write<int>(recoilParams + 0x88, one_i);
        mem.write<float>(recoilParams + 0x8C, zero_f);
        mem.write<int>(recoilParams + 0x94, one_i);
        mem.write<float>(recoilParams + 0x98, zero_f);
        mem.write<int>(recoilParams + 0xA0, one_i);
        mem.write<float>(recoilParams + 0xA4, zero_f);
        mem.write<int>(recoilParams + 0xAC, one_i);
        mem.write<float>(recoilParams + 0xB0, zero_f);
        mem.write<int>(recoilParams + 0xB8, one_i);
        mem.write<int>(recoilParams + 0xBC, zero_i);
        
        
        mem.write<int>(recoilData + 0x50, one_i);    
        mem.write<float>(recoilData + 0x54, zero_f); 
        mem.write<int>(recoilData + 0x58, one_i);
        mem.write<float>(recoilData + 0x5C, zero_f);
        mem.write<int>(recoilData + 0x60, one_i);
        mem.write<float>(recoilData + 0x64, zero_f);
        
        
        mem.write<float>(recoilData + 0x10, zero_f);  
        mem.write<float>(recoilData + 0x14, zero_f);  
        mem.write<float>(recoilData + 0x18, zero_f);  
        mem.write<float>(recoilData + 0x2C, zero_f);  
        mem.write<float>(recoilData + 0x38, zero_f);  
        mem.write<float>(recoilData + 0x68, zero_f);  
    }
    
    
    if (silentSettings.noRecoilData && !skipNoRecoil) {
        
        mem.write<float>(recoilData + 0x30, zero_f);  
        mem.write<float>(recoilData + 0x34, zero_f);  
    }
    
    
    if (silentSettings.noAccuracyData && !skipNoRecoil) {
        uint64_t accuracyData = mem.read<uint64_t>(gunController + 0x220);
        if (accuracyData) {
            mem.write<float>(accuracyData + 0x10, zero_f);
            mem.write<float>(accuracyData + 0x14, zero_f);
        }
    }
    
    
    if (silentSettings.noGunControllerFloats && !skipNoRecoil) {
        bool zero_b = false;
        mem.write<float>(gunController + 0x238, zero_f);
        mem.write<float>(gunController + 0x23C, zero_f);
        mem.write<float>(gunController + 0x240, zero_f);
        mem.write<float>(gunController + 0x21C, zero_f);
        mem.write<bool>(gunController + 0x218, zero_b);
        mem.write<bool>(gunController + 0x244, zero_b);
        mem.write<float>(gunController + 0x170, zero_f);
        mem.write<float>(gunController + 0x174, zero_f);
        
        
        mem.write<int>(gunController + 0x1E4, zero_i);
        mem.write<int>(gunController + 0x1E8, zero_i);
        mem.write<int>(gunController + 0x1EC, zero_i);
        mem.write<int>(gunController + 0x1F0, zero_i);
        mem.write<int>(gunController + 0x1F4, zero_i);
        mem.write<int>(gunController + 0x1F8, zero_i);
        mem.write<int>(gunController + 0x1FC, zero_i);
        mem.write<int>(gunController + 0x200, zero_i);
    }
    
    
    if (silentSettings.modifyGunParams && !skipNoRecoil) {
        int hundred_i = 100;
        mem.write<int>(gunParameters + 0x150, hundred_i);
        mem.write<int>(gunParameters + 0x240, one_i);
        mem.write<int>(gunParameters + 0x244, hundred_i);
    }
    
    
    Vector3 cameraPos = GetCameraPosition(localPlayer);
    Vector2 currentAngles = GetCurrentAimAngles(localPlayer);
    
    uint64_t ptr1 = mem.read<uint64_t>(localPlayer + offsets::viewMatrix_ptr1);
    if (!ptr1) return;
    uint64_t ptr2 = mem.read<uint64_t>(ptr1 + offsets::viewMatrix_ptr2);
    if (!ptr2) return;
    uint64_t ptr3 = mem.read<uint64_t>(ptr2 + offsets::viewMatrix_ptr3);
    if (!ptr3) return;
    Matrix viewMatrix = mem.read<Matrix>(ptr3 + offsets::viewMatrix_ptr4);
    
    uint64_t playersList = mem.read<uint64_t>(playerManager + offsets::entityList);
    if (!playersList) return;
    
    int playersCount = mem.read<int>(playersList + offsets::entityList_count);
    int localTeam = players.getTeam(localPlayer);
    
    float screenW = DisplayCache::GetScreenWidth();
    float screenH = DisplayCache::GetScreenHeight();
    float centerX = screenW / 2.0f;
    float centerY = screenH / 2.0f;
    
    float closestDist = FLT_MAX;
    Vector3 targetHead = Vector3::Zero();
    
    for (int i = 0; i < playersCount; i++) {
        uint64_t player = mem.read<uint64_t>(
            mem.read<uint64_t>(playersList + offsets::player_ptr1) + 
            offsets::player_ptr2 + offsets::player_ptr3 * i
        );
        
        if (!player || player == localPlayer) continue;
        if (players.getTeam(player) == localTeam) continue;
        if (players.getHealth(player) <= 0) continue;
        
        Vector3 headPos = aimbot.GetBonePosition(player, 0);
        if (headPos == Vector3::Zero()) continue;
        
        Vector3 screen = worldToScreen3(viewMatrix, headPos);
        if (screen.z <= 0) continue;
        
        float dx = screen.x - centerX;
        float dy = screen.y - centerY;
        float screenDist = sqrt(dx * dx + dy * dy);
        
        if (screenDist > silentSettings.fov) continue;
        
        float worldDist = (headPos - cameraPos).length();
        if (worldDist > silentSettings.maxDistance) continue;
        
        if (screenDist < closestDist) {
            closestDist = screenDist;
            targetHead = headPos;
        }
    }
    
    
    Vector2 recoilValue(0, 0);
    
    if (targetHead != Vector3::Zero()) {
        Vector3 dir = targetHead - cameraPos;
        float dist = dir.length();
        
        if (dist > 0.0001f) {
            float targetPitch = -asin(dir.y / dist) * (180.0f / M_PI);
            float targetYaw = atan2(dir.x, dir.z) * (180.0f / M_PI);
            
            float pitchDelta = NormalizeAngle(targetPitch - currentAngles.x);
            float yawDelta = NormalizeAngle(targetYaw - currentAngles.y);
            
            recoilValue = Vector2(-pitchDelta * dynamicMultiplier, yawDelta * dynamicMultiplier);
        }
    }
    
    
    mem.write<float>(recoilData + 0x1C, recoilValue.x);
    mem.write<float>(recoilData + 0x20, recoilValue.y);
    mem.write<float>(recoilData + 0x24, recoilValue.x);
    mem.write<float>(recoilData + 0x28, recoilValue.y);
    
    Vector2 currentRecoil = mem.read<Vector2>(recoilData + 0x1C);
    
    if (silentSettings.showDebug) {
        ImGui::GetBackgroundDrawList()->AddCircle(
            ImVec2(centerX, centerY), silentSettings.fov,
            IM_COL32(255, 255, 255, 50), 64, 1.0f);
        
        if (targetHead != Vector3::Zero()) {
            Vector3 screen = worldToScreen3(viewMatrix, targetHead);
            if (screen.z > 0) {
                ImGui::GetBackgroundDrawList()->AddCircle(
                    ImVec2(screen.x, screen.y), 8.0f, 
                    IM_COL32(255, 0, 255, 200), 12, 2.0f);
            }
        }
        
        char dbg[512];
        snprintf(dbg, sizeof(dbg), 
            "Silent | %s | Tgt: %s\n"
            "Write: %.1f, %.1f | Read: %.1f, %.1f\n"
            "Mult: %.2f | CamDev: %.3f\n"
            "RecoilParams: %s (0x%llX)\n"
            "GunState: %d | Skip: %s",
            weaponName.c_str(),
            targetHead != Vector3::Zero() ? "YES" : "NO",
            recoilValue.x, recoilValue.y,
            currentRecoil.x, currentRecoil.y,
            dynamicMultiplier, camDevCoeff,
            recoilParamsFromData ? "fromData" : "fromGun",
            (unsigned long long)recoilParams,
            gunState,
            skipNoRecoil ? "YES" : "NO");
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(50, 300), 
            IM_COL32(255, 255, 255, 200), dbg);
    }
}
