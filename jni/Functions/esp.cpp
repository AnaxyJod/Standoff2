#include "../includes/uses.h"
#include "../includes/players.h"
#include "../menu/ui.h"

void g_functions::g_esp::Update(Vector3 Position, int health, int team, int localTeam, int armor, int ping, int money, std::string name, std::string weapon, uint64_t player, uint64_t localPlayer) 
{
    if (!functions.esp.enabled) return;
    if (health <= 0) return;
    if (menu.team_check && team == localTeam) return;
    if (Position == Vector3::Zero()) return;

    bool isTopVisible = false;
    bool isBottomVisible = false;

    ImVec2 screenTop = worldToScreen(Position + Vector3(0, 1.67f, 0), functions.viewMatrix, &isTopVisible);
    ImVec2 screenBottom = worldToScreen(Position, functions.viewMatrix, &isBottomVisible);

    if (!isTopVisible || !isBottomVisible) return;

    float boxHeight = fabsf(screenTop.y - screenBottom.y);
    float boxWidth = boxHeight / functions.esp.box_width_ratio;

    ImVec2 boxMin = ImVec2(screenTop.x - boxWidth, screenTop.y);
    ImVec2 boxMax = ImVec2(screenTop.x + boxWidth, screenBottom.y);

    float centerX = (boxMin.x + boxMax.x) * 0.5f;
    
    auto drawList = ImGui::GetBackgroundDrawList();

    
    if (functions.esp.box_filled && functions.esp.box) {
        ImU32 filled_color = IM_COL32(
            (int)(functions.esp.filled_color[0] * 255),
            (int)(functions.esp.filled_color[1] * 255),
            (int)(functions.esp.filled_color[2] * 255),
            (int)(functions.esp.filled_color[3] * 255)
        );
        drawList->AddRectFilled(boxMin, boxMax, filled_color);
    }

    
    if (functions.esp.box) {
        ImU32 box_color = IM_COL32(
            (int)(functions.esp.box_color[0] * 255),
            (int)(functions.esp.box_color[1] * 255),
            (int)(functions.esp.box_color[2] * 255),
            (int)(functions.esp.box_color[3] * 255)
        );
        
        if (functions.esp.box_outline_enabled) {
            drawList->AddRect(boxMin, boxMax, IM_COL32(0, 0, 0, 255), 0.0f, 0, functions.esp.box_width + functions.esp.box_outline_width);
        }
        drawList->AddRect(boxMin, boxMax, box_color, 0.0f, 0, functions.esp.box_width);
    }

    
    if (functions.esp.health_bar) {
        ImU32 health_bar_color = IM_COL32(
            (int)(functions.esp.health_bar_color[0] * 255),
            (int)(functions.esp.health_bar_color[1] * 255),
            (int)(functions.esp.health_bar_color[2] * 255),
            (int)(functions.esp.health_bar_color[3] * 255)
        );
        
        float barHeight = boxHeight;
        float barWidth = 4.0f;
        float barX = boxMin.x - (6.0f * 1.194f); 
        
        ImVec2 barBg_min = ImVec2(barX, boxMin.y);
        ImVec2 barBg_max = ImVec2(barX + barWidth, boxMax.y);
        
        
        if (functions.esp.health_bar_outline_enabled) {
            drawList->AddRectFilled(
                ImVec2(barBg_min.x - 1, barBg_min.y - 1),
                ImVec2(barBg_max.x + 1, barBg_max.y + 1),
                IM_COL32(0, 0, 0, 255)
            );
        }
        drawList->AddRectFilled(barBg_min, barBg_max, IM_COL32(50, 50, 50, 200));
        
        
        float healthPercent = (health > 100) ? 1.0f : (health < 0) ? 0.0f : health * 0.01f;
        float healthBarHeight = barHeight * healthPercent;
        
        if (functions.esp.health_bar_gradient) {
            
            ImU32 topColor = IM_COL32(255, 255, 255, 255);    
            ImU32 bottomColor = IM_COL32(0, 0, 0, 255);       
            
            drawList->AddRectFilledMultiColor(
                ImVec2(barX, boxMax.y - healthBarHeight),
                ImVec2(barX + barWidth, boxMax.y),
                topColor, topColor, bottomColor, bottomColor
            );
        } else {
            drawList->AddRectFilled(
                ImVec2(barX, boxMax.y - healthBarHeight),
                ImVec2(barX + barWidth, boxMax.y),
                health_bar_color
            );
        }
    }

    
    if (functions.esp.armor_bar) {
        ImU32 armor_bar_color = IM_COL32(
            (int)(functions.esp.armor_bar_color[0] * 255),
            (int)(functions.esp.armor_bar_color[1] * 255),
            (int)(functions.esp.armor_bar_color[2] * 255),
            (int)(functions.esp.armor_bar_color[3] * 255)
        );
        
        float barWidth = boxWidth * 2.0f;
        float barHeight = 4.0f;
        float barY = boxMax.y + (6.0f * 0.512f); 
        
        ImVec2 barBg_min = ImVec2(boxMin.x, barY);
        ImVec2 barBg_max = ImVec2(boxMax.x, barY + barHeight);
        
        
        if (functions.esp.armor_bar_outline_enabled) {
            drawList->AddRectFilled(
                ImVec2(barBg_min.x - 1, barBg_min.y - 1),
                ImVec2(barBg_max.x + 1, barBg_max.y + 1),
                IM_COL32(0, 0, 0, 255)
            );
        }
        drawList->AddRectFilled(barBg_min, barBg_max, IM_COL32(50, 50, 50, 200));
        
        
        float armorPercent = (armor > 100) ? 1.0f : (armor < 0) ? 0.0f : armor * 0.01f;
        float armorBarWidth = barWidth * armorPercent;
        
        if (functions.esp.armor_bar_gradient) {
            
            ImU32 leftColor = IM_COL32(255, 255, 255, 255);  
            ImU32 rightColor = IM_COL32(0, 0, 0, 255);       
            drawList->AddRectFilledMultiColor(
                ImVec2(boxMin.x, barY),
                ImVec2(boxMin.x + armorBarWidth, barY + barHeight),
                leftColor, rightColor, rightColor, leftColor
            );
        } else {
            drawList->AddRectFilled(
                ImVec2(boxMin.x, barY),
                ImVec2(boxMin.x + armorBarWidth, barY + barHeight),
                armor_bar_color
            );
        }
    }

    
    if (functions.esp.skeleton) {
        ImU32 skeleton_color = IM_COL32(
            (int)(functions.esp.skeleton_color[0] * 255),
            (int)(functions.esp.skeleton_color[1] * 255),
            (int)(functions.esp.skeleton_color[2] * 255),
            (int)(functions.esp.skeleton_color[3] * 255)
        );
        
        
        uint64_t view = mem.read<uint64_t>(player + offsets::playerCharacterView);
        if (view) {
            
            static int updateCounter = 0;
            if (updateCounter++ % 3 == 0) { 
                mem.write<bool>(view + offsets::viewVisible, true);
            }
            
            uint64_t bipedmap = mem.read<uint64_t>(view + offsets::bipedMap);
            if (bipedmap) {
                
                uint64_t boneOffsets[13] = {
                    offsets::bone_head,           
                    offsets::bone_neck,           
                    offsets::bone_hip,            
                    offsets::bone_leftShoulder,   
                    offsets::bone_leftUpperArm,   
                    offsets::bone_leftHand,       
                    offsets::bone_rightShoulder,  
                    offsets::bone_rightUpperArm,  
                    offsets::bone_rightHand,      
                    offsets::bone_leftUpperLeg,   
                    offsets::bone_leftLowerLeg,   
                    offsets::bone_rightUpperLeg,  
                    offsets::bone_rightLowerLeg   
                };
                
                ImVec2 points[13];
                bool boneVisible[13] = {false};
                
                
                for (int i = 0; i < 13; i++) {
                    uint64_t transform = mem.read<uint64_t>(bipedmap + boneOffsets[i]);
                    if (transform) {
                        Vector3 worldPos = GameString::GetPosition(transform);
                        Vector3 screenPos = FastWorldToScreen(worldPos, functions.viewMatrix);
                        if (screenPos.z > 0.01f) {
                            points[i] = ImVec2(screenPos.x, screenPos.y);
                            boneVisible[i] = true;
                        }
                    }
                }
                
                
                if (functions.esp.skeleton_outline_enabled) {
                    
                    if (boneVisible[0] && boneVisible[1]) 
                        drawList->AddLine(points[0], points[1], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[1] && boneVisible[3]) 
                        drawList->AddLine(points[1], points[3], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[1] && boneVisible[6]) 
                        drawList->AddLine(points[1], points[6], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[3] && boneVisible[4]) 
                        drawList->AddLine(points[3], points[4], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[4] && boneVisible[5]) 
                        drawList->AddLine(points[4], points[5], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[6] && boneVisible[7]) 
                        drawList->AddLine(points[6], points[7], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[7] && boneVisible[8]) 
                        drawList->AddLine(points[7], points[8], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[1] && boneVisible[2]) 
                        drawList->AddLine(points[1], points[2], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[2] && boneVisible[9]) 
                        drawList->AddLine(points[2], points[9], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[9] && boneVisible[10]) 
                        drawList->AddLine(points[9], points[10], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[2] && boneVisible[11]) 
                        drawList->AddLine(points[2], points[11], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                    if (boneVisible[11] && boneVisible[12]) 
                        drawList->AddLine(points[11], points[12], IM_COL32(0, 0, 0, 255), functions.esp.skeleton_width + 2.0f);
                }
                
                
                if (boneVisible[0] && boneVisible[1]) 
                    drawList->AddLine(points[0], points[1], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[1] && boneVisible[3]) 
                    drawList->AddLine(points[1], points[3], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[1] && boneVisible[6]) 
                    drawList->AddLine(points[1], points[6], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[3] && boneVisible[4]) 
                    drawList->AddLine(points[3], points[4], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[4] && boneVisible[5]) 
                    drawList->AddLine(points[4], points[5], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[6] && boneVisible[7]) 
                    drawList->AddLine(points[6], points[7], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[7] && boneVisible[8]) 
                    drawList->AddLine(points[7], points[8], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[1] && boneVisible[2]) 
                    drawList->AddLine(points[1], points[2], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[2] && boneVisible[9]) 
                    drawList->AddLine(points[2], points[9], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[9] && boneVisible[10]) 
                    drawList->AddLine(points[9], points[10], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[2] && boneVisible[11]) 
                    drawList->AddLine(points[2], points[11], skeleton_color, functions.esp.skeleton_width);
                if (boneVisible[11] && boneVisible[12]) 
                    drawList->AddLine(points[11], points[12], skeleton_color, functions.esp.skeleton_width);
                
                
            }
        }
    }

    
    if (functions.esp.name && !name.empty()) {
        ImU32 name_color = IM_COL32(
            (int)(functions.esp.name_color[0] * 255),
            (int)(functions.esp.name_color[1] * 255),
            (int)(functions.esp.name_color[2] * 255),
            (int)(functions.esp.name_color[3] * 255)
        );
        
        ImFont* nameFont = font_pixel ? font_pixel : ImGui::GetFont();
        ImVec2 textSize = nameFont->CalcTextSizeA(nameFont->FontSize, FLT_MAX, 0.0f, name.c_str());
        ImVec2 textPos = ImVec2(centerX - textSize.x * 0.5f, boxMin.y - textSize.y - 2.0f);
        
        
        drawList->AddText(nameFont, nameFont->FontSize, ImVec2(textPos.x - 1, textPos.y), IM_COL32(0, 0, 0, 255), name.c_str());
        drawList->AddText(nameFont, nameFont->FontSize, ImVec2(textPos.x + 1, textPos.y), IM_COL32(0, 0, 0, 255), name.c_str());
        drawList->AddText(nameFont, nameFont->FontSize, ImVec2(textPos.x, textPos.y - 1), IM_COL32(0, 0, 0, 255), name.c_str());
        drawList->AddText(nameFont, nameFont->FontSize, ImVec2(textPos.x, textPos.y + 1), IM_COL32(0, 0, 0, 255), name.c_str());
        drawList->AddText(nameFont, nameFont->FontSize, textPos, name_color, name.c_str());
    }

    
    if (functions.esp.money) {
        ImU32 money_color = IM_COL32(255, 255, 255, 255); 
        
        ImFont* moneyFont = font_pixel ? font_pixel : ImGui::GetFont();
        char moneyText[32];
        sprintf(moneyText, "$%d", money);
        
        ImVec2 textSize = moneyFont->CalcTextSizeA(moneyFont->FontSize, FLT_MAX, 0.0f, moneyText);
        ImVec2 textPos = ImVec2(boxMax.x + 4.0f, boxMin.y);
        
        
        drawList->AddText(moneyFont, moneyFont->FontSize, ImVec2(textPos.x - 1, textPos.y), IM_COL32(0, 0, 0, 255), moneyText);
        drawList->AddText(moneyFont, moneyFont->FontSize, ImVec2(textPos.x + 1, textPos.y), IM_COL32(0, 0, 0, 255), moneyText);
        drawList->AddText(moneyFont, moneyFont->FontSize, ImVec2(textPos.x, textPos.y - 1), IM_COL32(0, 0, 0, 255), moneyText);
        drawList->AddText(moneyFont, moneyFont->FontSize, ImVec2(textPos.x, textPos.y + 1), IM_COL32(0, 0, 0, 255), moneyText);
        drawList->AddText(moneyFont, moneyFont->FontSize, textPos, money_color, moneyText);
    }
}
