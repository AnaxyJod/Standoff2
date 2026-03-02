#pragma once
#include "imgui/imgui/imgui.h"
#include <map>
#include <string>


struct KeyBind {
    ImGuiKey key = ImGuiKey_None;
    bool useCtrl = false;
    bool useAlt = false;
    bool useShift = false;
    bool isBinding = false;
    bool justStartedBinding = false; 
    std::string displayName;
};


class KeyBindManager {
private:
    std::map<std::string, KeyBind> binds;
    std::string currentBinding = "";
    
public:
    
    void AddBind(const std::string& id, const std::string& displayName, ImGuiKey defaultKey = ImGuiKey_None) {
        if (binds.find(id) == binds.end()) {
            binds[id] = {defaultKey, false, false, false, false, false, displayName};
        }
    }
    
    
    void StartBinding(const std::string& id) {
        
        if (!currentBinding.empty() && binds.find(currentBinding) != binds.end()) {
            binds[currentBinding].isBinding = false;
            binds[currentBinding].justStartedBinding = false;
        }
        
        currentBinding = id;
        if (binds.find(id) != binds.end()) {
            binds[id].isBinding = true;
            binds[id].justStartedBinding = true;
        }
    }
    
    
    void ProcessBinding() {
        if (currentBinding.empty() || binds.find(currentBinding) == binds.end()) {
            return;
        }
        
        ImGuiIO& io = ImGui::GetIO();
        
        
        if (binds[currentBinding].justStartedBinding) {
            binds[currentBinding].justStartedBinding = false;
            return;
        }
        
        
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            binds[currentBinding].isBinding = false;
            currentBinding = "";
            return;
        }
        
        
        bool ctrlPressed = io.KeyCtrl;
        bool altPressed = io.KeyAlt;
        bool shiftPressed = io.KeyShift;
        
        
        
        for (int key = ImGuiKey_A; key <= ImGuiKey_Z; key++) {
            if (ImGui::IsKeyPressed((ImGuiKey)key, false)) {
                binds[currentBinding].key = (ImGuiKey)key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        for (int key = ImGuiKey_0; key <= ImGuiKey_9; key++) {
            if (ImGui::IsKeyPressed((ImGuiKey)key, false)) {
                binds[currentBinding].key = (ImGuiKey)key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        for (int key = ImGuiKey_Keypad0; key <= ImGuiKey_Keypad9; key++) {
            if (ImGui::IsKeyPressed((ImGuiKey)key, false)) {
                binds[currentBinding].key = (ImGuiKey)key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        for (int key = ImGuiKey_F1; key <= ImGuiKey_F12; key++) {
            if (ImGui::IsKeyPressed((ImGuiKey)key, false)) {
                binds[currentBinding].key = (ImGuiKey)key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        const ImGuiKey specialKeys[] = {
            ImGuiKey_Space, ImGuiKey_Apostrophe, ImGuiKey_Comma, ImGuiKey_Minus,
            ImGuiKey_Period, ImGuiKey_Slash, ImGuiKey_Semicolon, ImGuiKey_Equal,
            ImGuiKey_LeftBracket, ImGuiKey_Backslash, ImGuiKey_RightBracket,
            ImGuiKey_GraveAccent, ImGuiKey_CapsLock, ImGuiKey_ScrollLock,
            ImGuiKey_NumLock, ImGuiKey_PrintScreen, ImGuiKey_Pause,
            ImGuiKey_KeypadDecimal, ImGuiKey_KeypadDivide, ImGuiKey_KeypadMultiply,
            ImGuiKey_KeypadSubtract, ImGuiKey_KeypadAdd, ImGuiKey_KeypadEnter,
            ImGuiKey_KeypadEqual,
            ImGuiKey_Menu, ImGuiKey_Tab, ImGuiKey_Enter, ImGuiKey_Backspace,
            ImGuiKey_Delete, ImGuiKey_Home, ImGuiKey_End,
            ImGuiKey_PageUp, ImGuiKey_PageDown, ImGuiKey_LeftArrow,
            ImGuiKey_RightArrow, ImGuiKey_UpArrow, ImGuiKey_DownArrow
        };
        
        for (ImGuiKey key : specialKeys) {
            if (ImGui::IsKeyPressed(key, false)) {
                binds[currentBinding].key = key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        
        
        for (int n = 0; n < ImGuiKey_NamedKey_COUNT; n++) {
            ImGuiKey key = (ImGuiKey)(ImGuiKey_NamedKey_BEGIN + n);
            
            
            if (key == ImGuiKey_Escape) continue;
            
            
            
            if (key == ImGuiKey_MouseLeft || key == ImGuiKey_MouseRight) continue;
            
            if (ImGui::IsKeyPressed(key, false)) {
                binds[currentBinding].key = key;
                binds[currentBinding].useCtrl = ctrlPressed;
                binds[currentBinding].useAlt = altPressed;
                binds[currentBinding].useShift = shiftPressed;
                binds[currentBinding].isBinding = false;
                currentBinding = "";
                return;
            }
        }
        
        
        if (io.MouseWheel > 0.0f) {
            binds[currentBinding].key = ImGuiKey_MouseWheelY; 
            binds[currentBinding].useCtrl = ctrlPressed;
            binds[currentBinding].useAlt = altPressed;
            binds[currentBinding].useShift = shiftPressed;
            binds[currentBinding].isBinding = false;
            currentBinding = "";
            return;
        }
        if (io.MouseWheel < 0.0f) {
            binds[currentBinding].key = ImGuiKey_MouseWheelY; 
            binds[currentBinding].useCtrl = ctrlPressed;
            binds[currentBinding].useAlt = altPressed;
            binds[currentBinding].useShift = shiftPressed;
            binds[currentBinding].isBinding = false;
            currentBinding = "";
            return;
        }
    }
    
    
    bool IsBindPressed(const std::string& id) {
        if (binds.find(id) == binds.end()) return false;
        if (binds[id].key == ImGuiKey_None) return false;
        
        ImGuiIO& io = ImGui::GetIO();
        
        
        if (binds[id].useCtrl && !io.KeyCtrl) return false;
        if (binds[id].useAlt && !io.KeyAlt) return false;
        if (binds[id].useShift && !io.KeyShift) return false;
        
        
        if (!binds[id].useCtrl && io.KeyCtrl) return false;
        if (!binds[id].useAlt && io.KeyAlt) return false;
        if (!binds[id].useShift && io.KeyShift) return false;
        
        return ImGui::IsKeyPressed(binds[id].key, false);
    }
    
    
    std::string GetKeyText(const std::string& id) {
        if (binds.find(id) == binds.end()) return "None";
        
        if (binds[id].isBinding) {
            return "Press key...";
        }
        
        if (binds[id].key == ImGuiKey_None) {
            return "None";
        }
        
        std::string result = "";
        
        
        if (binds[id].useCtrl) result += "Ctrl+";
        if (binds[id].useAlt) result += "Alt+";
        if (binds[id].useShift) result += "Shift+";
        
        
        const char* keyName = ImGui::GetKeyName(binds[id].key);
        if (keyName) {
            result += keyName;
        } else {
            result += "Unknown";
        }
        
        return result;
    }
    
    
    bool IsBinding(const std::string& id) {
        if (binds.find(id) == binds.end()) return false;
        return binds[id].isBinding;
    }
    
    
    bool RenderBindButton(const std::string& id) {
        std::string buttonText = GetKeyText(id);
        bool clicked = ImGui::Button(buttonText.c_str());
        
        if (clicked) {
            StartBinding(id);
        }
        
        return clicked;
    }
};

inline KeyBindManager keyBindManager;
