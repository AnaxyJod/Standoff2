#pragma once
#include "uses.h"
#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <deque>


struct HitLogEntry {
    std::string attackerName;      
    std::string victimName;        
    int damage;                    
    std::string weaponName;        
    bool isHeadshot;               
    bool isKill;                   
    std::chrono::steady_clock::time_point timestamp; 
    float displayDuration;         
};

class HitLogger {
public:
    
    bool enabled = false;              
    bool showHeadshotIcon = false;     
    bool showKillIcon = false;         
    bool showWeapon = false;           
    bool showDamage = false;           
    
    float displayTime = 3.0f;          
    float fadeDelay = 2.0f;            
    int maxEntries = 10;               
    
    
    float textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float damageColor[4] = { 1.0f, 0.3f, 0.3f, 1.0f };
    float headshotColor[4] = { 1.0f, 0.8f, 0.0f, 1.0f };
    float killColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float weaponColor[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
    float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    
    
    float positionX = 45.0f;           
    float positionY = 50.0f;           
    
    
    void LogHit(const std::string& attackerName, 
                const std::string& victimName, 
                int damage, 
                const std::string& weaponName,
                bool isHeadshot = false,
                bool isKill = false);
    
    void Update();                     
    void Render();                     
    
    
    const std::deque<HitLogEntry>& GetEntries() const { return entries; }
    
private:
    std::deque<HitLogEntry> entries;
    std::mutex entriesMutex;
    
    void RemoveExpiredEntries();
};


inline HitLogger hitLogger;
