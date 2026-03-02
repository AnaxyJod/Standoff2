#include "ui.h"
#include "../includes/uses.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/fonts.h"
#include "../includes/aim.h"
#include "../includes/rcs.h"
#include "../includes/skinchanger.h"
#include "../includes/keybinds.h"

void g_menu::render(){
    
    static bool bindsInitialized = false;
    if (!bindsInitialized) {
        bindsInitialized = true;
        
        keyBindManager.AddBind("esp", "ESP");
        keyBindManager.AddBind("aimbot", "Aimbot");
        keyBindManager.AddBind("rcs", "RCS");
        keyBindManager.AddBind("norecoil", "No Recoil");
        keyBindManager.AddBind("infinityammo", "Infinity Ammo");
        keyBindManager.AddBind("moneyhack", "Money Hack");
        keyBindManager.AddBind("wallshot", "Wallshot");
        keyBindManager.AddBind("firerate", "Fire Rate");
        
        
        keyBindManager.AddBind("teamcheck", "Team Check");
        keyBindManager.AddBind("box", "Box");
        keyBindManager.AddBind("boxoutline", "Box Outline");
        keyBindManager.AddBind("boxfilled", "Box Filled");
        keyBindManager.AddBind("healthbar", "Health Bar");
        keyBindManager.AddBind("hpbaroutline", "HP Bar Outline");
        keyBindManager.AddBind("hpbargradient", "HP Bar Gradient");
        keyBindManager.AddBind("armorbar", "Armor Bar");
        keyBindManager.AddBind("armorbaroutline", "Armor Bar Outline");
        keyBindManager.AddBind("armorbargradient", "Armor Bar Gradient");
        keyBindManager.AddBind("name", "Name");
        keyBindManager.AddBind("money", "Money");
        keyBindManager.AddBind("skeleton", "Skeleton");
        keyBindManager.AddBind("skeletonoutline", "Skeleton Outline");
        keyBindManager.AddBind("fovcheck", "FOV Check");
        keyBindManager.AddBind("drawfov", "Draw FOV");
        keyBindManager.AddBind("fovoutline", "FOV Outline");
        keyBindManager.AddBind("vischeck", "Visibility Check");
        keyBindManager.AddBind("firecheck", "Fire Check");
        keyBindManager.AddBind("knifebot", "Knife Bot");
        keyBindManager.AddBind("triggerbot", "Trigger Bot");
        keyBindManager.AddBind("nospread", "No Spread");
    }
    
    
    keyBindManager.ProcessBinding();
    
    
    if (!functions.show_menu && menu.keybinds_enabled) {
        if (keyBindManager.IsBindPressed("esp")) {
            functions.esp.enabled = !functions.esp.enabled;
        }
        if (keyBindManager.IsBindPressed("aimbot")) {
            aimSettings.enabled = !aimSettings.enabled;
        }
        if (keyBindManager.IsBindPressed("rcs")) {
            recoilSettings.enabled = !recoilSettings.enabled;
        }
        if (keyBindManager.IsBindPressed("norecoil")) {
            functions.noRecoil = !functions.noRecoil;
        }
        if (keyBindManager.IsBindPressed("infinityammo")) {
            functions.infinityAmmo = !functions.infinityAmmo;
        }
        if (keyBindManager.IsBindPressed("moneyhack")) {
            functions.moneyHack = !functions.moneyHack;
        }
        if (keyBindManager.IsBindPressed("wallshot")) {
            functions.wallshot = !functions.wallshot;
        }
        if (keyBindManager.IsBindPressed("firerate")) {
            functions.fireRate = !functions.fireRate;
        }
        
        
        if (menu.show_settings_binds) {
            if (keyBindManager.IsBindPressed("teamcheck")) {
                menu.team_check = !menu.team_check;
            }
            if (keyBindManager.IsBindPressed("box")) {
                functions.esp.box = !functions.esp.box;
            }
            if (keyBindManager.IsBindPressed("boxoutline")) {
                functions.esp.box_outline_enabled = !functions.esp.box_outline_enabled;
            }
            if (keyBindManager.IsBindPressed("boxfilled")) {
                functions.esp.box_filled = !functions.esp.box_filled;
            }
            if (keyBindManager.IsBindPressed("healthbar")) {
                functions.esp.health_bar = !functions.esp.health_bar;
            }
            if (keyBindManager.IsBindPressed("hpbaroutline")) {
                functions.esp.health_bar_outline_enabled = !functions.esp.health_bar_outline_enabled;
            }
            if (keyBindManager.IsBindPressed("hpbargradient")) {
                functions.esp.health_bar_gradient = !functions.esp.health_bar_gradient;
            }
            if (keyBindManager.IsBindPressed("armorbar")) {
                functions.esp.armor_bar = !functions.esp.armor_bar;
            }
            if (keyBindManager.IsBindPressed("armorbaroutline")) {
                functions.esp.armor_bar_outline_enabled = !functions.esp.armor_bar_outline_enabled;
            }
            if (keyBindManager.IsBindPressed("armorbargradient")) {
                functions.esp.armor_bar_gradient = !functions.esp.armor_bar_gradient;
            }
            if (keyBindManager.IsBindPressed("name")) {
                functions.esp.name = !functions.esp.name;
            }
            if (keyBindManager.IsBindPressed("money")) {
                functions.esp.money = !functions.esp.money;
            }
            if (keyBindManager.IsBindPressed("skeleton")) {
                functions.esp.skeleton = !functions.esp.skeleton;
            }
            if (keyBindManager.IsBindPressed("skeletonoutline")) {
                functions.esp.skeleton_outline_enabled = !functions.esp.skeleton_outline_enabled;
            }
            if (keyBindManager.IsBindPressed("fovcheck")) {
                aimSettings.fovCheck = !aimSettings.fovCheck;
            }
            if (keyBindManager.IsBindPressed("drawfov")) {
                aimSettings.drawFov = !aimSettings.drawFov;
            }
            if (keyBindManager.IsBindPressed("fovoutline")) {
                aimSettings.drawFovOutline = !aimSettings.drawFovOutline;
            }
            if (keyBindManager.IsBindPressed("vischeck")) {
                aimSettings.checkVisible = !aimSettings.checkVisible;
            }
            if (keyBindManager.IsBindPressed("firecheck")) {
                aimSettings.fireCheck = !aimSettings.fireCheck;
            }
            if (keyBindManager.IsBindPressed("knifebot")) {
                aimSettings.knifeBot = !aimSettings.knifeBot;
            }
            if (keyBindManager.IsBindPressed("triggerbot")) {
                aimSettings.triggerBot = !aimSettings.triggerBot;
            }
            if (keyBindManager.IsBindPressed("nospread")) {
                recoilSettings.noSpread = !recoilSettings.noSpread;
            }
        }
    }
    
    
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        functions.show_menu = !functions.show_menu;
    }

    if (functions.show_menu)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
        
        if (ImGui::Begin("zov", nullptr, window_flags))
        {
            
            if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("esp"))
                {
                    menu.current_tab = 0;
                ImGui::Checkbox("esp", &functions.esp.enabled);
                keyBindManager.RenderBindButton("esp");
                
                ImGui::Checkbox("team check", &team_check);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("teamcheck");
                }
                
                ImGui::Checkbox("box", &functions.esp.box);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("box");
                }
                ImGui::Checkbox("box outline", &functions.esp.box_outline_enabled);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("boxoutline");
                }
                
                ImGui::Checkbox("box filled", &functions.esp.box_filled);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("boxfilled");
                }
                
                ImGui::Checkbox("health bar", &functions.esp.health_bar);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("healthbar");
                }
                if (functions.esp.health_bar) {
                    ImGui::Checkbox("hp bar outline", &functions.esp.health_bar_outline_enabled);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("hpbaroutline");
                    }
                    ImGui::Checkbox("hp bar gradient", &functions.esp.health_bar_gradient);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("hpbargradient");
                    }
                }
                
                ImGui::Checkbox("armor bar", &functions.esp.armor_bar);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("armorbar");
                }
                if (functions.esp.armor_bar) {
                    ImGui::Checkbox("armor bar outline", &functions.esp.armor_bar_outline_enabled);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("armorbaroutline");
                    }
                    ImGui::Checkbox("armor bar gradient", &functions.esp.armor_bar_gradient);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("armorbargradient");
                    }
                }
                
                ImGui::Checkbox("name", &functions.esp.name);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("name");
                }
                ImGui::Checkbox("money", &functions.esp.money);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("money");
                }
                ImGui::Checkbox("skeleton", &functions.esp.skeleton);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("skeleton");
                }
                if (functions.esp.skeleton) {
                    ImGui::Checkbox("skeleton outline", &functions.esp.skeleton_outline_enabled);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("skeletonoutline");
                    }
                }
                
                ImGui::EndTabItem();
            }
            
            
            if (ImGui::BeginTabItem("aim"))
            {
                menu.current_tab = 1;
                ImGui::Checkbox("aimbot", &aimSettings.enabled);
                keyBindManager.RenderBindButton("aimbot");
                
                if (aimSettings.enabled) {
                    ImGui::Checkbox("fov check", &aimSettings.fovCheck);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("fovcheck");
                    }
                    if (aimSettings.fovCheck) {
                        ImGui::SliderFloat("fov", &aimSettings.fov, 10.0f, 300.0f, "%.0f");
                    }
                    ImGui::Checkbox("draw fov", &aimSettings.drawFov);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("drawfov");
                    }
                    if (aimSettings.drawFov) {
                        ImGui::Checkbox("fov outline", &aimSettings.drawFovOutline);
                        if (menu.show_settings_binds) {
                            keyBindManager.RenderBindButton("fovoutline");
                        }
                    }
                    
                    ImGui::Checkbox("visibility check", &aimSettings.checkVisible);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("vischeck");
                    }
                    ImGui::Checkbox("fire check", &aimSettings.fireCheck);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("firecheck");
                    }
                    ImGui::Checkbox("knife bot", &aimSettings.knifeBot);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("knifebot");
                    }
                    
                    ImGui::SliderFloat("smooth", &aimSettings.smooth, 0.1f, 20.0f, "%.1f");
                    
                    
                    const char* boneNames[] = {
                        "Head", "Neck", "Chest", "Hip", 
                        "Left Shoulder", "Left Arm", "Left Hand",
                        "Right Shoulder", "Right Arm", "Right Hand",
                        "Left Leg", "Left Foot", 
                        "Right Leg", "Right Foot"
                    };
                    
                    
                    std::string selectedText = "";
                    bool hasSelected = false;
                    for (int i = 0; i < 14; i++) {
                        if (aimSettings.selectedBones[i]) {
                            if (hasSelected) selectedText += ", ";
                            selectedText += boneNames[i];
                            hasSelected = true;
                        }
                    }
                    if (!hasSelected) selectedText += "None";
                    
                    if (ImGui::BeginCombo("target bones", selectedText.c_str())) {
                        ImDrawList* drawList = ImGui::GetWindowDrawList();
                        
                        for (int i = 0; i < 14; i++) {
                            bool isSelected = aimSettings.selectedBones[i];
                            
                            
                            ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
                            
                            
                            ImGui::PushID(i);
                            char label[32];
                            sprintf(label, "##bone_%d", i);
                            if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, ImGui::GetTextLineHeightWithSpacing()))) {
                                aimSettings.selectedBones[i] = !aimSettings.selectedBones[i];
                            }
                            ImGui::PopID();
                            
                            
                            ImVec2 textPos = ImVec2(
                                cursorScreenPos.x,
                                cursorScreenPos.y
                            );
                            
                            ImU32 textColor = IM_COL32(255, 255, 255, 255);
                            drawList->AddText(textPos, textColor, boneNames[i]);
                        }
                        ImGui::EndCombo();
                    }
                    
                    ImGui::Checkbox("trigger bot", &aimSettings.triggerBot);
                    if (menu.show_settings_binds) {
                        keyBindManager.RenderBindButton("triggerbot");
                    }
                    if (aimSettings.triggerBot) {
                        ImGui::SliderFloat("trigger delay", &aimSettings.triggerDelay, 0.0f, 1.0f, "%.2f");
                    }
                }
                
                ImGui::EndTabItem();
            }
            
            
            if (ImGui::BeginTabItem("misc"))
            {
                menu.current_tab = 2;
                
                
                ImGui::Checkbox("host indicator", &menu.show_host_indicator);
                
                ImGui::Separator();
                
                
                ImGui::Checkbox("rcs", &recoilSettings.enabled);
                keyBindManager.RenderBindButton("rcs");
                
                if (recoilSettings.enabled) {
                    ImGui::SliderFloat("horizontal", &recoilSettings.horizontalValue, 0.0f, 10.0f, "%.1f");
                    ImGui::SliderFloat("vertical", &recoilSettings.verticalValue, 0.0f, 10.0f, "%.1f");
                }
                
                ImGui::Checkbox("no spread", &recoilSettings.noSpread);
                if (menu.show_settings_binds) {
                    keyBindManager.RenderBindButton("nospread");
                }
                ImGui::Checkbox("no recoil", &functions.noRecoil);
                keyBindManager.RenderBindButton("norecoil");
                
                ImGui::Separator();
                ImGui::Checkbox("infinity ammo", &functions.infinityAmmo);
                keyBindManager.RenderBindButton("infinityammo");
                
                ImGui::Checkbox("money hack", &functions.moneyHack);
                keyBindManager.RenderBindButton("moneyhack");
                
                ImGui::Checkbox("wallshot", &functions.wallshot);
                keyBindManager.RenderBindButton("wallshot");
                
                ImGui::Checkbox("fire rate", &functions.fireRate);
                keyBindManager.RenderBindButton("firerate");
                

                
                ImGui::EndTabItem();
            }
            
            
            if (ImGui::BeginTabItem("skins"))
            {
                menu.current_tab = 3;
                const char* allSkins[] = {
                    
                    "M9 Bayonet Blue Blood", "Karambit Claw", "Butterfly Legacy", "jKommando Acid", 
                    "Kunai Bone", "Scorpion Veil", "Flip Dragon Glass", "Tanto Dojo",
                    
                    "Gloves Phoenix Risen", "Gloves Autumn", "Gloves Geometric",
                    "Gloves Retro Wave", "Gloves Living Flame", "Gloves Neuro",
                    "Gloves Burning Fists", "Gloves Punk", "Gloves Champion",
                    "Gloves Steam Rider", "Gloves Year Of The Tiger", "Gloves Acid",
                    "Gloves Camo", "Gloves Fossil", "Gloves Handcraft",
                    "Gloves Raider", "Gloves Thug", "Gloves Templar",
                    "Gloves X-Ray", "Gloves Onyx", "Gloves Tan Hide"
                };
                
                if (ImGui::BeginCombo("skin", allSkins[functions.selectedKnifeSkin])) {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    
                    for (int i = 0; i < IM_ARRAYSIZE(allSkins); i++) {
                        bool isSelected = (functions.selectedKnifeSkin == i);
                        
                        ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
                        
                        ImGui::PushID(i);
                        char label[32];
                        sprintf(label, "##skin_%d", i);
                        if (ImGui::Selectable(label, isSelected, 0, ImVec2(0, ImGui::GetTextLineHeightWithSpacing()))) {
                            functions.selectedKnifeSkin = i;
                        }
                        ImGui::PopID();
                        
                        ImVec2 textPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y);
                        ImU32 textColor = IM_COL32(255, 255, 255, 255);
                        drawList->AddText(textPos, textColor, allSkins[i]);
                    }
                    ImGui::EndCombo();
                }
                
                if (ImGui::Button("change skin")) {
                    ApplyKnifeSkin();
                }
                
                ImGui::EndTabItem();
            }
            
            
            if (ImGui::BeginTabItem("settings"))
            {
                menu.current_tab = 4;
                ImGui::Separator();
                
                if (ImGui::Checkbox("run", &run)) {
                    if (!run) {
                        should_exit = true;
                    }
                }
                
                ImGui::Checkbox("keybinds", &menu.keybinds_enabled);
                ImGui::Checkbox("settings", &menu.show_settings_binds);
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }
}