#pragma once
#include "uses.h"
#include <unordered_map>
#include <vector>


struct CircleEffect {
    float timer = 0.0f;
    float startDelay = 0.0f;          
    float alpha = 255.0f;
    float radius = 1.0f;
    bool isPlaying = false;           
    bool isWaiting = false;           
    Vector3 startPosition;            
};


class FootstepCircle {
    private:
        struct PlayerData {
            std::vector<CircleEffect> circles;
            float nextCircleDelay = 0.0f;
            float timeSinceLastStart = 0.0f;
            bool hasActiveCircle = false;
            Vector3 lastPosition; 
        };
        
        std::unordered_map<uint64_t, PlayerData> playerData;
        
        
        
    public:
        
        void UpdateEffect(uint64_t playerPtr, bool triggerNow, const Vector3& currentPos) {
            auto& data = playerData[playerPtr];
            
            
            bool isMoving = false;
            if (data.lastPosition != Vector3::Zero()) {
                float distanceMoved = (currentPos - data.lastPosition).length();
                isMoving = (distanceMoved > 0.05f); 
            }
            data.lastPosition = currentPos;
            
            
            bool shouldTrigger = triggerNow || (isMoving && functions.esp.footstep_exist);
            
            if (shouldTrigger) {
                
                if (!data.hasActiveCircle || data.timeSinceLastStart >= functions.esp.footstep_delta) {
                    
                    CreateCircle(data, currentPos, 0.0f);
                    data.timeSinceLastStart = 0.0f;
                    data.hasActiveCircle = true;
                }
                
                data.nextCircleDelay = functions.esp.footstep_delta;
            }
        }
        
        
        void CreateCircle(PlayerData& data, const Vector3& position, float delay) {
            CircleEffect newCircle;
            newCircle.startDelay = delay;
            newCircle.isWaiting = (delay > 0.0f);
            newCircle.isPlaying = !newCircle.isWaiting;
            newCircle.timer = 0.0f;
            newCircle.startPosition = position;
            data.circles.push_back(newCircle);
        }
        
        
        void Draw3DCircle(uint64_t playerPtr, const Matrix& camera, 
                         const float footstepColor[4], float deltaTime) {
            auto it = playerData.find(playerPtr);
            if (it == playerData.end()) return;
            
            auto& data = it->second;
            data.timeSinceLastStart += deltaTime;
            
            
            if (data.nextCircleDelay > 0.0f) {
                data.nextCircleDelay -= deltaTime;
                if (data.nextCircleDelay <= 0.0f) {
                    
                    Vector3 lastPosition = GetLastCirclePosition(data);
                    if (lastPosition != Vector3::Zero()) {
                        CreateCircle(data, lastPosition, 0.0f);
                        data.timeSinceLastStart = 0.0f;
                        data.nextCircleDelay = 0.0f;
                    }
                }
            }
        
        
        for (size_t i = 0; i < data.circles.size(); ) {
            auto& circle = data.circles[i];
            
            if (circle.isWaiting) {
                
                circle.startDelay -= deltaTime;
                if (circle.startDelay <= 0.0f) {
                    circle.isWaiting = false;
                    circle.isPlaying = true;
                }
                ++i;
            }
            else if (circle.isPlaying) {
                
                circle.timer += deltaTime;
                
                
                if (circle.timer > functions.esp.footstep_speed) {
                    
                    data.circles.erase(data.circles.begin() + i);
                    continue;
                }
                
                
                float t = circle.timer / functions.esp.footstep_speed;
                
                
                float baseAlpha = footstepColor[3] * 255.0f;
                circle.alpha = (1.0f - t) * baseAlpha;
                
                
                circle.radius = t * 1.5f;
                
                
                if (circle.alpha >= 5.0f) {
                    DrawSingleCircle(circle, camera, footstepColor);
                }
                
                ++i;
            }
            else {
                ++i;
            }
        }
        
        
        data.hasActiveCircle = false;
        for (const auto& circle : data.circles) {
            if (circle.isPlaying) {
                data.hasActiveCircle = true;
                break;
            }
        }
        
    }
    
    
    Vector3 GetLastCirclePosition(PlayerData& data) {
        if (!data.circles.empty()) {
            return data.circles.back().startPosition;
        }
        return Vector3::Zero();
    }
    
    
    void DrawSingleCircle(CircleEffect& circle, const Matrix& camera, const float footstepColor[4]) {
        const int segments = 32;
        Vector3 base = circle.startPosition;
        base.y -= 0.1f;
        
        
        ImU32 circleColor = IM_COL32(
            (int)(footstepColor[0] * 255), 
            (int)(footstepColor[1] * 255), 
            (int)(footstepColor[2] * 255), 
            (int)circle.alpha
        );
        
        auto drawList = ImGui::GetBackgroundDrawList();
        
        for (int s = 0; s < segments; ++s) {
            float angle1 = (2 * M_PI / segments) * s;
            float angle2 = (2 * M_PI / segments) * (s + 1);
            
            Vector3 p1 = {
                base.x + cosf(angle1) * circle.radius,
                base.y,
                base.z + sinf(angle1) * circle.radius
            };
            Vector3 p2 = {
                base.x + cosf(angle2) * circle.radius,
                base.y,
                base.z + sinf(angle2) * circle.radius
            };
            
            Vector3 sp1 = FastWorldToScreen(p1, camera);
            Vector3 sp2 = FastWorldToScreen(p2, camera);
            
            if (sp1.z > 0.001f && sp2.z > 0.001f) {
                drawList->AddLine(
                    ImVec2(sp1.x, sp1.y),
                    ImVec2(sp2.x, sp2.y),
                    circleColor,
                    1.5f
                );
            }
        }
    }
    
    
    void RemovePlayer(uint64_t playerPtr) {
        playerData.erase(playerPtr);
    }
    
    
    void ClearAll() {
        playerData.clear();
    }
    
    
    bool IsAnimationPlaying(uint64_t playerPtr) {
        auto it = playerData.find(playerPtr);
        if (it != playerData.end()) {
            return it->second.hasActiveCircle;
        }
        return false;
    }
    
    void CleanupInactivePlayers(float currentTime) {
        static float lastCleanupTime = 0.0f;
        if (currentTime - lastCleanupTime < 5.0f) return; 
        
        for (auto it = playerData.begin(); it != playerData.end(); ) {
            
            if (it->second.circles.empty() && currentTime - lastCleanupTime > 10.0f) {
                it = playerData.erase(it);
            } else {
                ++it;
            }
        }
        
        lastCleanupTime = currentTime;
    }
};


inline FootstepCircle footstepCircle;