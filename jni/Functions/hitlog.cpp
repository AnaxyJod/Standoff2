#include "../includes/hitlog.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/imgui/draw/draw.h"
#include "../includes/sans_font.h"
#include <sstream>


static const float HITLOG_FONT_SIZE = 45.0f;

void HitLogger::LogHit(const std::string& attackerName, 
                       const std::string& victimName, 
                       int damage, 
                       const std::string& weaponName,
                       bool isHeadshot,
                       bool isKill) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(entriesMutex);
    
    HitLogEntry entry;
    entry.attackerName = attackerName;
    entry.victimName = victimName;
    entry.damage = damage;
    entry.weaponName = weaponName;
    entry.isHeadshot = isHeadshot;
    entry.isKill = isKill;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.displayDuration = displayTime;
    
    
    entries.push_front(entry);
    
    
    while (entries.size() > static_cast<size_t>(maxEntries * 2)) {
        entries.pop_back();
    }
}

void HitLogger::Update() {
    if (!enabled) return;
    
    RemoveExpiredEntries();
}

void HitLogger::RemoveExpiredEntries() {
    std::lock_guard<std::mutex> lock(entriesMutex);
    
    auto now = std::chrono::steady_clock::now();
    
    
    
    while (!entries.empty()) {
        auto& oldest = entries.back();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - oldest.timestamp).count() / 1000.0f;
        
        float totalTime = fadeDelay + oldest.displayDuration;
        if (elapsed > totalTime) {
            entries.pop_back();
        } else {
            break;
        }
    }
}

void HitLogger::Render() {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(entriesMutex);
    
    if (entries.empty()) return;
    
    auto displayInfo = draw::getDisplayInfo();
    float screenWidth = static_cast<float>(displayInfo.width);
    float screenHeight = static_cast<float>(displayInfo.height);
    
    
    float startX = screenWidth * (positionX / 100.0f);
    float startY = screenHeight * (positionY / 100.0f);
    float lineHeight = HITLOG_FONT_SIZE + 4.0f;
    float padding = 5.0f;
    
    auto now = std::chrono::steady_clock::now();
    auto drawList = ImGui::GetBackgroundDrawList();
    int displayedCount = 0;
    
    for (const auto& entry : entries) {
        if (displayedCount >= maxEntries) break;
        
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - entry.timestamp).count() / 1000.0f;
        
        
        
        
        float alpha = 1.0f;
        
        if (elapsed > fadeDelay) {
            
            float fadeElapsed = elapsed - fadeDelay;
            if (entry.displayDuration > 0.0f) {
                alpha = 1.0f - (fadeElapsed / entry.displayDuration);
            }
        }
        
        
        alpha = std::max(0.0f, std::min(1.0f, alpha));
        
        if (alpha <= 0.0f) continue;
        
        
        std::string namePart = entry.victimName;
        std::string damagePart = showDamage ? " [-" + std::to_string(entry.damage) + "]" : "";
        std::string weaponPart = (showWeapon && !entry.weaponName.empty()) ? " (" + entry.weaponName + ")" : "";
        std::string hsPart = (entry.isHeadshot && showHeadshotIcon) ? " [HS]" : "";
        std::string killPart = (entry.isKill && showKillIcon) ? " [KILL]" : "";
        
        
        ImVec2 nameSize = sans->CalcTextSizeA(HITLOG_FONT_SIZE, FLT_MAX, 0.0f, namePart.c_str());
        ImVec2 damageSize = sans->CalcTextSizeA(HITLOG_FONT_SIZE, FLT_MAX, 0.0f, damagePart.c_str());
        ImVec2 weaponSize = sans->CalcTextSizeA(HITLOG_FONT_SIZE, FLT_MAX, 0.0f, weaponPart.c_str());
        ImVec2 hsSize = sans->CalcTextSizeA(HITLOG_FONT_SIZE, FLT_MAX, 0.0f, hsPart.c_str());
        ImVec2 killSize = sans->CalcTextSizeA(HITLOG_FONT_SIZE, FLT_MAX, 0.0f, killPart.c_str());
        
        float totalWidth = nameSize.x + damageSize.x + weaponSize.x + hsSize.x + killSize.x + padding * 2;
        
        float y = startY + displayedCount * (lineHeight + 2.0f);
        
        
        ImU32 bgColor = IM_COL32(
            (int)(backgroundColor[0] * 255),
            (int)(backgroundColor[1] * 255),
            (int)(backgroundColor[2] * 255),
            (int)(backgroundColor[3] * 255 * alpha)
        );
        
        drawList->AddRectFilled(
            ImVec2(startX, y),
            ImVec2(startX + totalWidth, y + lineHeight),
            bgColor,
            3.0f
        );
        
        
        ImU32 nameColor = IM_COL32(
            (int)(textColor[0] * 255),
            (int)(textColor[1] * 255),
            (int)(textColor[2] * 255),
            (int)(textColor[3] * 255 * alpha)
        );
        
        ImU32 dmgColor = IM_COL32(
            (int)(damageColor[0] * 255),
            (int)(damageColor[1] * 255),
            (int)(damageColor[2] * 255),
            (int)(damageColor[3] * 255 * alpha)
        );
        
        ImU32 hsColor = IM_COL32(
            (int)(headshotColor[0] * 255),
            (int)(headshotColor[1] * 255),
            (int)(headshotColor[2] * 255),
            (int)(headshotColor[3] * 255 * alpha)
        );
        
        ImU32 klColor = IM_COL32(
            (int)(killColor[0] * 255),
            (int)(killColor[1] * 255),
            (int)(killColor[2] * 255),
            (int)(killColor[3] * 255 * alpha)
        );
        
        float currentX = startX + padding;
        float textY = y + 2.0f;
        
        
        drawList->AddText(sans, HITLOG_FONT_SIZE, ImVec2(currentX, textY), nameColor, namePart.c_str());
        currentX += nameSize.x;
        
        
        if (!damagePart.empty()) {
            drawList->AddText(sans, HITLOG_FONT_SIZE, ImVec2(currentX, textY), dmgColor, damagePart.c_str());
            currentX += damageSize.x;
        }
        
        
        if (!weaponPart.empty()) {
            ImU32 wpnColor = IM_COL32(
                (int)(weaponColor[0] * 255),
                (int)(weaponColor[1] * 255),
                (int)(weaponColor[2] * 255),
                (int)(weaponColor[3] * 255 * alpha)
            );
            drawList->AddText(sans, HITLOG_FONT_SIZE, ImVec2(currentX, textY), wpnColor, weaponPart.c_str());
            currentX += weaponSize.x;
        }
        
        
        if (!hsPart.empty()) {
            drawList->AddText(sans, HITLOG_FONT_SIZE, ImVec2(currentX, textY), hsColor, hsPart.c_str());
            currentX += hsSize.x;
        }
        
        
        if (!killPart.empty()) {
            drawList->AddText(sans, HITLOG_FONT_SIZE, ImVec2(currentX, textY), klColor, killPart.c_str());
        }
        
        displayedCount++;
    }
}
