#pragma once
#include "uses.h"
#include "imgui/imgui/imgui.h"
#include <vector>
#include <chrono>
#include <mutex>
#include <cmath>


struct HitMarker {
    Vector3 startPos;       
    Vector3 hitPos;         
    int damage;             
    std::chrono::steady_clock::time_point timestamp;
    float duration;         
    bool isHeadshot;        
    bool isKill;            
};


struct HitMarkerSettings {
    bool enabled = false;
    int markerType = 0;                 
    float displayTime = 1.5f;           
    float fadeTime = 0.5f;              
    
    
    bool useHitTypeColors = false;
    
    
    float hitColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };       
    float headshotColor[4] = { 1.0f, 0.8f, 0.0f, 1.0f };  
    float killColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };      
    
    int maxMarkers = 20;
};


inline HitMarkerSettings hitMarkerSettings;

class HitMarkerSystem {
public:
    
    void AddMarker(const Vector3& start, const Vector3& hit, int damage, bool isHeadshot = false, bool isKill = false);
    
    
    void Update();
    
    
    void Render(const Matrix& viewMatrix);
    
    
    void RenderMarkers(const Matrix& viewMatrix);
    
    
    void RenderDamage(const Matrix& viewMatrix);
    
    
    void Clear();
    
private:
    std::vector<HitMarker> markers;
    std::mutex markersMutex;
    
    void RemoveExpiredMarkers();
};


inline HitMarkerSystem hitMarker;
