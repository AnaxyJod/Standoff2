#pragma once
#include "uses.h"
#include "players.h"
#include "aim.h"

struct SilentSettings {
    bool enabled = false;
    float fov = 150.0f;
    float maxDistance = 300.0f;
    float multiplier = 1.0f;
    bool showDebug = true;
    bool autoMultiplier = true;
    bool disableCamDev = false;
    
    
    bool noRecoilParams = false;      
    bool noRecoilData = false;        
    bool noAccuracyData = false;      
    bool noGunControllerFloats = false; 
    bool modifyGunParams = false;     
};

inline SilentSettings silentSettings;

class Silent {
public:
    void Render();
    void RenderMenu();
};

inline Silent silent;
