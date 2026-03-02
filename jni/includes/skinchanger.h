#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include "imgui/imgui/imgui.h"

struct SkinItem {
    int id;
    std::string name;
    int rarity;  
    uint64_t ptr;
};

struct SkinChangerSettings {
    bool enabled = false;
};

inline SkinChangerSettings skinChangerSettings;

class SkinChanger {
private:
    uint64_t libunity_base_;
    std::map<int, std::pair<std::string, int>> all_skins; 
    std::vector<SkinItem> inventory_skins;
    std::map<int, int> saved_skin_replacements; 
    
    
    int skin_tab = 0;
    int selected_inv_skin = -1;
    int selected_all_skin_idx = 0;
    std::string search_string;
    std::vector<std::pair<int, std::pair<std::string, int>>> sorted_all_skins;
    std::vector<std::pair<int, std::pair<std::string, int>>> filtered_skins;
    
    ImU32 GetRarityColor(int rarity);
    void UpdateFilteredSkins();
    std::string ReadUnityString(uint64_t str_ptr);

public:
    SkinChanger();
    void Init(uint64_t libunity_base);
    
    void UpdateAllSkins();
    void UpdateInventorySkins();
    void ReplaceSkin(SkinItem& to_swap, int new_id);
    void RenderUI();
    
    
    bool SaveConfig(const std::string& name);
    bool LoadConfig(const std::string& name);
    bool DeleteConfig(const std::string& name);
    std::vector<std::string> GetConfigList();
    void ApplySavedSkins();
    
    const std::vector<SkinItem>& GetInventorySkins() const { return inventory_skins; }
    const std::map<int, std::pair<std::string, int>>& GetAllSkins() const { return all_skins; }
};

inline SkinChanger skinChanger;
