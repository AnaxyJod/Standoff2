#include "../includes/skinchanger.h"
#include "../includes/uses.h"
#include "../includes/memory/helper.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/imgui/imgui/imgui_internal.h"
#include "../includes/keyboard.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdlib>
#include <ctime>

static const char* SKIN_CONFIG_DIR = "/sdcard/allah/skins/";


namespace SkinOffsets {
    
    constexpr int inventoryService_addr = 135588032;
    
    
    constexpr int allSkinsDict = 0xE8;          
    constexpr int inventorySkinsDict = 0xF8;    
    
    
    constexpr int itemDef_id = 0x10;            
    constexpr int itemDef_name = 0x18;          
    constexpr int itemDef_rarity = 0x20;        
    
    
    constexpr int invItem_id = 0x10;            
    constexpr int invItem_inventoryId = 0x14;   
}


template <typename TKey, typename TValue>
class UnityDictionary {
public:
    static int Count(uint64_t dict_ptr) {
        if (!dict_ptr) return 0;
        int ret = mem.read<int>(dict_ptr + 0x20);
        return ret >= 1 ? ret : 0;
    }
    
    static TValue GetValue(uint64_t dict_ptr, size_t idx) {
        uint64_t entries = mem.read<uint64_t>(dict_ptr + 0x18);
        return mem.read<TValue>(entries + 0x20 + 0x10 + 0x18 * idx);
    }
    
    static TKey GetKey(uint64_t dict_ptr, size_t idx) {
        uint64_t entries = mem.read<uint64_t>(dict_ptr + 0x18);
        return mem.read<TKey>(entries + 0x20 + 0x18 * idx);
    }
};

SkinChanger::SkinChanger() : libunity_base_(0) {}

void SkinChanger::Init(uint64_t libunity_base) {
    libunity_base_ = libunity_base;
}

std::string SkinChanger::ReadUnityString(uint64_t str_ptr) {
    if (!str_ptr) return "";
    int length = mem.read<int>(str_ptr + 0x10);
    if (length <= 0 || length > 256) return "";
    
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; i++) {
        char16_t ch = mem.read<char16_t>(str_ptr + 0x14 + i * 2);
        if (ch < 128) result += (char)ch;
        else result += '?';
    }
    return result;
}

ImU32 SkinChanger::GetRarityColor(int rarity) {
    switch (rarity) {
        case 1: return IM_COL32(230, 230, 230, 255);  
        case 2: return IM_COL32(45, 100, 130, 255);   
        case 3: return IM_COL32(0, 70, 180, 255);     
        case 4: return IM_COL32(102, 83, 154, 255);   
        case 5: return IM_COL32(105, 16, 82, 255);    
        case 6: return IM_COL32(255, 0, 0, 255);      
        case 7: return IM_COL32(255, 255, 0, 255);    
        default: return IM_COL32(230, 230, 230, 255);
    }
}

void SkinChanger::UpdateAllSkins() {
    
    uint64_t inventory_service = helper.getInstance(libunity_base_ + SkinOffsets::inventoryService_addr, true, 0x0);
    if (!inventory_service) {
        
        inventory_service = helper.getInstance(libunity_base_ + SkinOffsets::inventoryService_addr, false, 0x0);
    }
    if (!inventory_service) return;

    all_skins.clear();
    
    
    uint64_t item_def_dict = mem.read<uint64_t>(inventory_service + SkinOffsets::allSkinsDict);
    if (item_def_dict) {
        int count = UnityDictionary<int, uint64_t>::Count(item_def_dict);
        for (int i = 0; i < count && i < 5000; i++) {
            uint64_t item_def = UnityDictionary<int, uint64_t>::GetValue(item_def_dict, i);
            if (!item_def) continue;
            
            int id = mem.read<int>(item_def + SkinOffsets::itemDef_id);
            uint64_t name_ptr = mem.read<uint64_t>(item_def + SkinOffsets::itemDef_name);
            std::string name = ReadUnityString(name_ptr);
            
            int rarity = mem.read<int>(item_def + SkinOffsets::itemDef_rarity);
            if (rarity < 1 || rarity > 7) rarity = 1;
            
            if (!name.empty() && id > 0) {
                all_skins[id] = {name, rarity};
            }
        }
    }
}

void SkinChanger::UpdateInventorySkins() {
    uint64_t inventory_service = helper.getInstance(libunity_base_ + SkinOffsets::inventoryService_addr, true, 0x0);
    if (!inventory_service) {
        inventory_service = helper.getInstance(libunity_base_ + SkinOffsets::inventoryService_addr, false, 0x0);
    }
    if (!inventory_service) return;

    UpdateAllSkins();
    inventory_skins.clear();
    
    
    uint64_t skins_dict = mem.read<uint64_t>(inventory_service + SkinOffsets::inventorySkinsDict);
    if (skins_dict) {
        int count = UnityDictionary<int, uint64_t>::Count(skins_dict);
        for (int i = 0; i < count && i < 1000; i++) {
            uint64_t skin = UnityDictionary<int, uint64_t>::GetValue(skins_dict, i);
            if (!skin) continue;
            
            int id = mem.read<int>(skin + SkinOffsets::invItem_id);
            
            std::string name = "Skin #" + std::to_string(id);
            int rarity = 1;
            
            if (all_skins.count(id)) {
                name = all_skins[id].first;
                rarity = all_skins[id].second;
            }
            
            inventory_skins.push_back({id, name, rarity, skin});
        }
    }
    
    
    std::sort(inventory_skins.begin(), inventory_skins.end(), [](const SkinItem& a, const SkinItem& b) {
        return a.rarity > b.rarity;
    });
    
    
    sorted_all_skins.clear();
    for (auto& p : all_skins) {
        sorted_all_skins.push_back({p.first, p.second});
    }
    std::sort(sorted_all_skins.begin(), sorted_all_skins.end(), 
        [](const auto& a, const auto& b) { return a.second.second > b.second.second; });
    
    filtered_skins = sorted_all_skins;
    selected_all_skin_idx = 0;
}

void SkinChanger::ReplaceSkin(SkinItem& to_swap, int new_id) {
    mem.write<int>(to_swap.ptr + SkinOffsets::invItem_id, new_id);
    UpdateInventorySkins();
}

void SkinChanger::UpdateFilteredSkins() {
    filtered_skins.clear();
    std::string search_lower = search_string;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
    
    for (auto& skin : sorted_all_skins) {
        if (search_lower.empty()) {
            filtered_skins.push_back(skin);
        } else {
            std::string name_lower = skin.second.first;
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
            if (name_lower.find(search_lower) != std::string::npos) {
                filtered_skins.push_back(skin);
            }
        }
    }
    
    if (selected_all_skin_idx >= (int)filtered_skins.size()) {
        selected_all_skin_idx = filtered_skins.empty() ? 0 : (int)filtered_skins.size() - 1;
    }
}

void SkinChanger::RenderUI() {
    
    static bool first_load = true;
    if (first_load && libunity_base_ != 0) {
        first_load = false;
        UpdateInventorySkins();
    }
    
    ImGuiIO& io = ImGui::GetIO();
    float scale = io.DisplaySize.x / 2560.0f;
    float padding = 30.0f * scale;
    float dist = 72.0f * scale;
    float btnHeight = 40.0f * scale;
    float listHeight = 400.0f * scale;
    
    
    float menuSizeX = io.DisplaySize.x * 896.0f / 2560.0f;
    float mo = io.DisplaySize.x * 128.0f / 2560.0f;
    float menuSizeY = io.DisplaySize.y;
    float contentHeight = menuSizeY - dist * 6.5f;
    float contentWidth = menuSizeX - mo - padding * 2;
    
    
    ImGui::SetCursorPosY(dist * 2.5f);
    if (!ImGui::BeginChild("SChScroll", ImVec2(contentWidth, contentHeight), false, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        ImGui::EndChild();
        return;
    }
    
    ImGui::Indent(padding);
    
    
    if (ImGui::Button("Refresh", ImVec2(150 * scale, btnHeight))) {
        UpdateInventorySkins();
    }
    
    if (inventory_skins.empty()) {
        ImGui::TextDisabled("No skins loaded. Press Refresh.");
        ImGui::Unindent(padding);
        ImGui::EndChild();
        return;
    }
    
    ImGui::Dummy(ImVec2(0, dist * 0.3f));
    
    
    if (ImGui::BeginTabBar("SkinChangerTabs")) {
        if (ImGui::BeginTabItem("Inventory")) {
            ImGui::BeginChild("InvList", ImVec2(contentWidth - padding * 2, listHeight), true);
            for (int i = 0; i < (int)inventory_skins.size(); i++) {
                auto& skin = inventory_skins[i];
                bool is_selected = (selected_inv_skin == i);
                
                ImGui::PushStyleColor(ImGuiCol_Text, GetRarityColor(skin.rarity));
                if (ImGui::Selectable(skin.name.c_str(), is_selected)) {
                    selected_inv_skin = i;
                }
                ImGui::PopStyleColor();
            }
            ImGui::EndChild();
            
            if (selected_inv_skin >= 0 && selected_inv_skin < (int)inventory_skins.size()) {
                ImGui::Text("Selected: %s", inventory_skins[selected_inv_skin].name.c_str());
            }
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("All Skins")) {
            
            ImGui::SetNextItemWidth(contentWidth - padding * 2);
            if (Keyboard::InputField("Search", &search_string, scale)) {
                UpdateFilteredSkins();
            }
            
            static std::string last_search;
            if (last_search != search_string) {
                last_search = search_string;
                UpdateFilteredSkins();
            }
            
            ImGui::BeginChild("AllList", ImVec2(contentWidth - padding * 2, listHeight - 50 * scale), true);
            for (int i = 0; i < (int)filtered_skins.size(); i++) {
                auto& skin = filtered_skins[i];
                bool is_selected = (selected_all_skin_idx == i);
                
                ImGui::PushStyleColor(ImGuiCol_Text, GetRarityColor(skin.second.second));
                if (ImGui::Selectable(skin.second.first.c_str(), is_selected)) {
                    selected_all_skin_idx = i;
                }
                ImGui::PopStyleColor();
            }
            ImGui::EndChild();
            
            
            if (ImGui::Button("Apply Skin", ImVec2(200 * scale, btnHeight))) {
                if (selected_inv_skin >= 0 && selected_inv_skin < (int)inventory_skins.size() && 
                    !filtered_skins.empty() && selected_all_skin_idx < (int)filtered_skins.size()) {
                    ReplaceSkin(inventory_skins[selected_inv_skin], filtered_skins[selected_all_skin_idx].first);
                    selected_inv_skin = -1;
                }
            }
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Config")) {
            static int selected_config = -1;
            static std::vector<std::string> config_list;
            
            if (ImGui::Button("Refresh Configs", ImVec2(200 * scale, btnHeight))) {
                config_list = GetConfigList();
            }
            
            ImGui::BeginChild("ConfigList", ImVec2(contentWidth - padding * 2, listHeight - 100 * scale), true);
            for (int i = 0; i < (int)config_list.size(); i++) {
                bool is_selected = (selected_config == i);
                if (ImGui::Selectable(config_list[i].c_str(), is_selected)) {
                    selected_config = i;
                }
            }
            ImGui::EndChild();
            
            
            if (ImGui::Button("Save", ImVec2(100 * scale, btnHeight))) {
                static int config_counter = 0;
                std::string name = "skin_cfg_" + std::to_string(++config_counter);
                SaveConfig(name);
                config_list = GetConfigList();
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Load", ImVec2(100 * scale, btnHeight))) {
                if (selected_config >= 0 && selected_config < (int)config_list.size()) {
                    if (LoadConfig(config_list[selected_config])) {
                        ApplySavedSkins();
                    }
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(100 * scale, btnHeight))) {
                if (selected_config >= 0 && selected_config < (int)config_list.size()) {
                    DeleteConfig(config_list[selected_config]);
                    config_list = GetConfigList();
                    selected_config = -1;
                }
            }
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::Unindent(padding);
    ImGui::EndChild();
}

bool SkinChanger::SaveConfig(const std::string& name) {
    struct stat st = {0};
    if (stat(SKIN_CONFIG_DIR, &st) == -1) {
        mkdir("/sdcard/allah/", 0777);
        mkdir(SKIN_CONFIG_DIR, 0777);
    }
    
    std::string filepath = std::string(SKIN_CONFIG_DIR) + name + ".scfg";
    std::ofstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    int count = (int)inventory_skins.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(int));
    
    for (auto& skin : inventory_skins) {
        int skin_id = skin.id;
        file.write(reinterpret_cast<const char*>(&skin_id), sizeof(int));
    }
    
    file.close();
    return true;
}

bool SkinChanger::LoadConfig(const std::string& name) {
    std::string filepath = std::string(SKIN_CONFIG_DIR) + name + ".scfg";
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    saved_skin_replacements.clear();
    
    int count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(int));
    
    for (int i = 0; i < count; i++) {
        int skin_id = 0;
        file.read(reinterpret_cast<char*>(&skin_id), sizeof(int));
        saved_skin_replacements[i] = skin_id;
    }
    
    file.close();
    return true;
}

bool SkinChanger::DeleteConfig(const std::string& name) {
    std::string filepath = std::string(SKIN_CONFIG_DIR) + name + ".scfg";
    return remove(filepath.c_str()) == 0;
}

std::vector<std::string> SkinChanger::GetConfigList() {
    std::vector<std::string> configs;
    DIR* dir = opendir(SKIN_CONFIG_DIR);
    
    if (dir == nullptr) return configs;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename.length() > 5 && filename.substr(filename.length() - 5) == ".scfg") {
            configs.push_back(filename.substr(0, filename.length() - 5));
        }
    }
    
    closedir(dir);
    return configs;
}

void SkinChanger::ApplySavedSkins() {
    if (saved_skin_replacements.empty() || inventory_skins.empty()) return;
    
    int idx = 0;
    for (auto& skin : inventory_skins) {
        if (saved_skin_replacements.count(idx)) {
            int new_id = saved_skin_replacements[idx];
            if (new_id != skin.id) {
                mem.write<int>(skin.ptr + SkinOffsets::invItem_id, new_id);
            }
        }
        idx++;
    }
    
    UpdateInventorySkins();
}
