#pragma once

#include "imgui/imgui/imgui.h"
#include <string>
#include <map>

namespace Keyboard {

inline bool g_isOpen = false;
inline std::string* g_editString = nullptr;
inline std::string g_backupString;
inline const char* g_currentLabel = nullptr;
inline std::map<const char*, int> g_resultMap;
inline ImVec2 g_keyboardPos = ImVec2(-1, -1);
inline bool g_isDragging = false;
inline ImVec2 g_dragOffset = ImVec2(0, 0);


inline void Open(const char* label, std::string* value) {
    if (!label || !value) return;
    g_currentLabel = label;
    g_editString = value;
    g_backupString = *value;
    g_isOpen = true;
    g_resultMap[label] = 0;
}


inline void Close(bool apply) {
    if (g_editString && !apply) {
        *g_editString = g_backupString;
    }
    g_resultMap[g_currentLabel] = apply ? 1 : -1;
    g_isOpen = false;
    g_editString = nullptr;
    g_currentLabel = nullptr;
}


inline int GetResult(const char* label) {
    int result = g_resultMap[label];
    if (result != 0) g_resultMap[label] = 0;
    return result;
}


inline int InputField(const char* label, std::string* value, float scale = 1.0f) {
    if (!label || !value) return 0;
    
    bool isEmpty = value->empty();
    std::string displayText = isEmpty ? label : *value;
    
    ImU32 textColor = isEmpty ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255);
    
    float width = ImGui::GetContentRegionAvail().x;
    float height = 60.0f * scale;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(textColor).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.02f, 0.5f));
    
    char btnId[128];
    snprintf(btnId, sizeof(btnId), "%s##input_btn", displayText.c_str());
    
    bool clicked = ImGui::Button(btnId, ImVec2(width, height));
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    
    if (clicked) {
        Open(label, value);
    }
    
    return GetResult(label);
}


inline void Render(float scale = 1.0f) {
    if (!g_isOpen || !g_editString) return;
    
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    
    
    float keySize = 50.0f * scale;
    float keySpacing = 5.0f * scale;
    float keyboardW = 11 * keySize + 10 * keySpacing + 60 * scale;
    float keyboardH = 5 * keySize + 4 * keySpacing + 60 * scale;
    
    
    if (g_keyboardPos.x < 0 || g_keyboardPos.y < 0) {
        g_keyboardPos.x = (screenW - keyboardW) / 2;
        g_keyboardPos.y = (screenH - keyboardH) / 2;
    }
    
    ImGui::SetNextWindowPos(g_keyboardPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(keyboardW, keyboardH));
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.047f, 0.047f, 0.047f, 1.0f));
    ImGui::Begin("##keyboard", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
    ImGui::PopStyleColor();
    
    
    
    if (ImGui::IsKeyPressed(ImGuiKey_A)) g_editString->push_back('A');
    if (ImGui::IsKeyPressed(ImGuiKey_B)) g_editString->push_back('B');
    if (ImGui::IsKeyPressed(ImGuiKey_C)) g_editString->push_back('C');
    if (ImGui::IsKeyPressed(ImGuiKey_D)) g_editString->push_back('D');
    if (ImGui::IsKeyPressed(ImGuiKey_E)) g_editString->push_back('E');
    if (ImGui::IsKeyPressed(ImGuiKey_F)) g_editString->push_back('F');
    if (ImGui::IsKeyPressed(ImGuiKey_G)) g_editString->push_back('G');
    if (ImGui::IsKeyPressed(ImGuiKey_H)) g_editString->push_back('H');
    if (ImGui::IsKeyPressed(ImGuiKey_I)) g_editString->push_back('I');
    if (ImGui::IsKeyPressed(ImGuiKey_J)) g_editString->push_back('J');
    if (ImGui::IsKeyPressed(ImGuiKey_K)) g_editString->push_back('K');
    if (ImGui::IsKeyPressed(ImGuiKey_L)) g_editString->push_back('L');
    if (ImGui::IsKeyPressed(ImGuiKey_M)) g_editString->push_back('M');
    if (ImGui::IsKeyPressed(ImGuiKey_N)) g_editString->push_back('N');
    if (ImGui::IsKeyPressed(ImGuiKey_O)) g_editString->push_back('O');
    if (ImGui::IsKeyPressed(ImGuiKey_P)) g_editString->push_back('P');
    if (ImGui::IsKeyPressed(ImGuiKey_Q)) g_editString->push_back('Q');
    if (ImGui::IsKeyPressed(ImGuiKey_R)) g_editString->push_back('R');
    if (ImGui::IsKeyPressed(ImGuiKey_S)) g_editString->push_back('S');
    if (ImGui::IsKeyPressed(ImGuiKey_T)) g_editString->push_back('T');
    if (ImGui::IsKeyPressed(ImGuiKey_U)) g_editString->push_back('U');
    if (ImGui::IsKeyPressed(ImGuiKey_V)) g_editString->push_back('V');
    if (ImGui::IsKeyPressed(ImGuiKey_W)) g_editString->push_back('W');
    if (ImGui::IsKeyPressed(ImGuiKey_X)) g_editString->push_back('X');
    if (ImGui::IsKeyPressed(ImGuiKey_Y)) g_editString->push_back('Y');
    if (ImGui::IsKeyPressed(ImGuiKey_Z)) g_editString->push_back('Z');
    
    if (ImGui::IsKeyPressed(ImGuiKey_0)) g_editString->push_back('0');
    if (ImGui::IsKeyPressed(ImGuiKey_1)) g_editString->push_back('1');
    if (ImGui::IsKeyPressed(ImGuiKey_2)) g_editString->push_back('2');
    if (ImGui::IsKeyPressed(ImGuiKey_3)) g_editString->push_back('3');
    if (ImGui::IsKeyPressed(ImGuiKey_4)) g_editString->push_back('4');
    if (ImGui::IsKeyPressed(ImGuiKey_5)) g_editString->push_back('5');
    if (ImGui::IsKeyPressed(ImGuiKey_6)) g_editString->push_back('6');
    if (ImGui::IsKeyPressed(ImGuiKey_7)) g_editString->push_back('7');
    if (ImGui::IsKeyPressed(ImGuiKey_8)) g_editString->push_back('8');
    if (ImGui::IsKeyPressed(ImGuiKey_9)) g_editString->push_back('9');
    
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) g_editString->push_back(' ');
    if (ImGui::IsKeyPressed(ImGuiKey_Minus)) g_editString->push_back('-');
    if (ImGui::IsKeyPressed(ImGuiKey_Period)) g_editString->push_back('.');
    if (ImGui::IsKeyPressed(ImGuiKey_Comma)) g_editString->push_back(',');
    if (ImGui::IsKeyPressed(ImGuiKey_Slash)) g_editString->push_back('/');
    
    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (!g_editString->empty()) {
            g_editString->pop_back();
        }
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        Close(true);
        ImGui::End();
        return;
    }
    
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    
    
    bool hoverWindow = ImGui::IsMouseHoveringRect(winPos, ImVec2(winPos.x + winSize.x, winPos.y + winSize.y));
    bool hoverKeys = false;
    
    float startY = 20 * scale;
    float startX = 20 * scale;
    for (int row = 0; row < 4; row++) {
        float offsetX = (row == 2) ? keySize * 0.5f : 0;
        for (int i = 0; i < 11; i++) {
            float x = winPos.x + startX + offsetX + i * (keySize + keySpacing);
            float y = winPos.y + startY + row * (keySize + keySpacing);
            if (ImGui::IsMouseHoveringRect(ImVec2(x, y), ImVec2(x + keySize, y + keySize))) {
                hoverKeys = true;
            }
        }
    }
    
    float spaceY = winPos.y + startY + 4 * (keySize + keySpacing);
    float spaceW = 11 * keySize + 10 * keySpacing;
    if (ImGui::IsMouseHoveringRect(ImVec2(winPos.x + startX, spaceY), ImVec2(winPos.x + startX + spaceW, spaceY + keySize))) {
        hoverKeys = true;
    }
    
    float btnW = 60 * scale;
    float btnH = 40 * scale;
    ImVec2 xPos = ImVec2(winPos.x + winSize.x - 70 * scale, winPos.y + 10 * scale);
    if (ImGui::IsMouseHoveringRect(xPos, ImVec2(xPos.x + btnW, xPos.y + btnH))) {
        hoverKeys = true;
    }
    
    if (hoverWindow && !hoverKeys && ImGui::IsMouseClicked(0)) {
        g_isDragging = true;
        g_dragOffset = ImVec2(io.MousePos.x - winPos.x, io.MousePos.y - winPos.y);
    }
    
    if (g_isDragging) {
        if (ImGui::IsMouseDown(0)) {
            g_keyboardPos.x = io.MousePos.x - g_dragOffset.x;
            g_keyboardPos.y = io.MousePos.y - g_dragOffset.y;
        } else {
            g_isDragging = false;
        }
    }
    
    
    draw->AddRect(winPos, winPos + winSize, IM_COL32(0, 0, 0, 255), 0, 0, 1.5f * scale);
    draw->AddRect(winPos + ImVec2(4.5f * scale, 4.5f * scale), winPos + winSize - ImVec2(4.5f * scale, 4.5f * scale), 
        IM_COL32(30, 30, 30, 255), 0, 0, 8 * scale);
    draw->AddRect(winPos + ImVec2(8 * scale, 8 * scale), winPos + winSize - ImVec2(8 * scale, 8 * scale), 
        IM_COL32(20, 20, 20, 255), 0, 0, 7.5f * scale);
    draw->AddRect(winPos + ImVec2(12.5f * scale, 12.5f * scale), winPos + winSize - ImVec2(12.5f * scale, 12.5f * scale), 
        IM_COL32(30, 30, 30, 255), 0, 0, 4 * scale);
    draw->AddRect(winPos + ImVec2(15.5f * scale, 15.5f * scale), winPos + winSize - ImVec2(15.5f * scale, 15.5f * scale), 
        IM_COL32(0, 0, 0, 255), 0, 0, 1.5f * scale);
    
    
    ImU32 activeColor = IM_COL32(51, 102, 255, 255);
    ImU32 inactiveColor = IM_COL32(180, 180, 180, 255);
    
    xPos = ImVec2(winPos.x + winSize.x - 50 * scale, winPos.y + 5 * scale);
    bool xHover = ImGui::IsMouseHoveringRect(xPos, ImVec2(xPos.x + btnW, xPos.y + btnH));
    bool xActive = xHover && ImGui::IsMouseDown(0);
    
    ImGui::SetWindowFontScale(1.5f);
    ImVec2 xTextSize = ImGui::CalcTextSize("X");
    draw->AddText(ImVec2(xPos.x + (btnW - xTextSize.x) / 2, xPos.y + (btnH - xTextSize.y) / 2), 
        xActive ? activeColor : inactiveColor, "X");
    ImGui::SetWindowFontScale(1.0f);
    
    if (xHover && ImGui::IsMouseClicked(0)) {
        Close(true);
    }

    
    const char* rows[] = {
        "1234567890-",
        "QWERTYUIOP<",
        "ASDFGHJKL_",
        "ZXCVBNM,./"
    };
    
    float keysStartY = 30 * scale;
    float keysStartX = 25 * scale;
    
    ImGui::SetWindowFontScale(scale * 1.2f);
    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();
    
    for (int row = 0; row < 4; row++) {
        float offsetX = (row == 2) ? keySize * 0.5f : 0;
        const char* keys = rows[row];
        int len = strlen(keys);
        
        for (int i = 0; i < len; i++) {
            float x = winPos.x + keysStartX + offsetX + i * (keySize + keySpacing);
            float y = winPos.y + keysStartY + row * (keySize + keySpacing);
            
            ImVec2 keyMin(x, y);
            ImVec2 keyMax(x + keySize, y + keySize);
            
            bool hover = ImGui::IsMouseHoveringRect(keyMin, keyMax);
            bool active = hover && ImGui::IsMouseDown(0);
            
            ImU32 keyBg = active ? IM_COL32(50, 50, 50, 255) : 
                          hover ? IM_COL32(40, 40, 40, 255) : IM_COL32(30, 30, 30, 255);
            draw->AddRectFilled(keyMin, keyMax, keyBg);
            
            char keyChar[2] = {keys[i], 0};
            ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, keyChar);
            draw->AddText(font, fontSize, ImVec2(x + (keySize - textSize.x) / 2, y + (keySize - textSize.y) / 2), 
                IM_COL32(255, 255, 255, 255), keyChar);
            
            if (hover && ImGui::IsMouseClicked(0)) {
                if (keys[i] == '<') {
                    if (!g_editString->empty()) {
                        g_editString->pop_back();
                    }
                } else {
                    g_editString->push_back(keys[i]);
                }
            }
        }
    }
    
    
    float spY = winPos.y + keysStartY + 4 * (keySize + keySpacing);
    float spW = 11 * keySize + 10 * keySpacing;
    ImVec2 spaceMin(winPos.x + keysStartX, spY);
    ImVec2 spaceMax(winPos.x + keysStartX + spW, spY + keySize);
    
    bool spaceHover = ImGui::IsMouseHoveringRect(spaceMin, spaceMax);
    bool spaceActive = spaceHover && ImGui::IsMouseDown(0);
    
    ImU32 spaceBg = spaceActive ? IM_COL32(50, 50, 50, 255) : 
                   spaceHover ? IM_COL32(40, 40, 40, 255) : IM_COL32(30, 30, 30, 255);
    draw->AddRectFilled(spaceMin, spaceMax, spaceBg);
    
    const char* spaceText = "space";
    ImVec2 spaceTextSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, spaceText);
    draw->AddText(font, fontSize, ImVec2(spaceMin.x + (spW - spaceTextSize.x) / 2, spaceMin.y + (keySize - spaceTextSize.y) / 2), 
        IM_COL32(255, 255, 255, 255), spaceText);
    
    ImGui::SetWindowFontScale(1.0f);
    
    if (spaceHover && ImGui::IsMouseClicked(0)) {
        g_editString->push_back(' ');
    }
    
    ImGui::End();
}

} 
