#include "../includes/rainbow.h"
#include <cmath>

RainbowManager* RainbowManager::instance = nullptr;

RainbowManager& RainbowManager::Get() {
    if (!instance) {
        instance = new RainbowManager();
    }
    return *instance;
}

void RainbowManager::Update(float speed) {
    
    hue += speed * 0.001f;
    if (hue > 1.0f) hue -= 1.0f;
    
    
    ImVec4 rainbowColor = ImColor::HSV(hue, 1.0f, 1.0f);
    currentRainbowColor = ImColor(rainbowColor);
    
    currentRainbowColorArray[0] = rainbowColor.x;
    currentRainbowColorArray[1] = rainbowColor.y;
    currentRainbowColorArray[2] = rainbowColor.z;
    currentRainbowColorArray[3] = 1.0f; 
}

ImU32 RainbowManager::GetRainbowColorWithOffset(float speed, float offset) {
    float currentHue = fmod(hue + offset, 1.0f);
    return ImColor::HSV(currentHue, 1.0f, 1.0f);
}

void RainbowManager::GetRainbowColorArrayWithOffset(float color[4], float speed, float offset) {
    float currentHue = fmod(hue + offset, 1.0f);
    ImVec4 rainbowColor = ImColor::HSV(currentHue, 1.0f, 1.0f);
    
    color[0] = rainbowColor.x;
    color[1] = rainbowColor.y;
    color[2] = rainbowColor.z;
    
}