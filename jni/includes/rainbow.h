#pragma once
#include "imgui/imgui/imgui.h"

class RainbowManager {
private:
    static RainbowManager* instance;
    float hue = 0.0f;
    
public:
    static RainbowManager& Get();
    
    
    ImU32 currentRainbowColor;
    float currentRainbowColorArray[4];
    
    
    void Update(float speed);
    
    
    ImU32 GetRainbowColorWithOffset(float speed, float offset = 0.0f);
    void GetRainbowColorArrayWithOffset(float color[4], float speed, float offset = 0.0f);
};