#pragma once
#include "../includes/uses.h"
#include "../includes/imgui/imgui/imgui_internal.h"
#include "../includes/fonts.h"
#include <map>
#include <atomic>
#include <vector>

class g_menu
{
private:
    struct Snowflake {
        float x, y;
        float size;
        float speed;
        float oscillation;
        float phase;
    };
    
    std::vector<Snowflake> snowflakes;
    void InitializeSnow();
    void UpdateSnow();
    void RenderSnow();

public:
    std::atomic<bool> should_exit;
    bool run = true;
    bool team_check = true;
    bool keybinds_enabled = false;  
    bool show_settings_binds = false;
    bool show_host_indicator = false;  
    int current_tab = 0;  
    void render();
};
inline g_menu menu;