#include "../includes/hitmarker.h"
#include "../includes/tracer.h"  
#include "../includes/hitmarker_font.h"  
#include "../includes/sans_font.h"  
#include <algorithm>
#include <string>

void HitMarkerSystem::AddMarker(const Vector3& start, const Vector3& hit, int damage, bool isHeadshot, bool isKill) {
    if (!hitMarkerSettings.enabled) return;
    
    std::lock_guard<std::mutex> lock(markersMutex);
    
    HitMarker marker;
    marker.startPos = start;
    marker.hitPos = hit;
    marker.damage = damage;
    marker.timestamp = std::chrono::steady_clock::now();
    marker.duration = hitMarkerSettings.displayTime;
    marker.isHeadshot = isHeadshot;
    marker.isKill = isKill;
    
    markers.push_back(marker);
    
    
    while (markers.size() > static_cast<size_t>(hitMarkerSettings.maxMarkers)) {
        markers.erase(markers.begin());
    }
}

void HitMarkerSystem::Update() {
    if (!hitMarkerSettings.enabled) return;
    RemoveExpiredMarkers();
}

void HitMarkerSystem::RemoveExpiredMarkers() {
    std::lock_guard<std::mutex> lock(markersMutex);
    
    auto now = std::chrono::steady_clock::now();
    
    markers.erase(
        std::remove_if(markers.begin(), markers.end(),
            [&now](const HitMarker& marker) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - marker.timestamp).count() / 1000.0f;
                return elapsed > marker.duration;
            }),
        markers.end()
    );
}

void HitMarkerSystem::Render(const Matrix& viewMatrix) {
    
    RenderMarkers(viewMatrix);
    RenderDamage(viewMatrix);
}


static const float HITMARKER_MIN_FONT_SIZE = 55.0f;  
static const float DAMAGE_MIN_FONT_SIZE = 57.0f;     


static void RenderMarkersInternal(const std::vector<HitMarker>& markers, const Matrix& viewMatrix, bool renderDamage) {
    if (markers.empty()) return;
    
    auto now = std::chrono::steady_clock::now();
    auto drawList = ImGui::GetBackgroundDrawList();
    
    for (const auto& marker : markers) {
        
        bool isDamageMarker = (hitMarkerSettings.markerType == 1);
        if (renderDamage != isDamageMarker) continue;
        
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - marker.timestamp).count() / 1000.0f;
        
        
        float alpha = 1.0f;
        float fadeStart = marker.duration - hitMarkerSettings.fadeTime;
        if (elapsed > fadeStart && hitMarkerSettings.fadeTime > 0.0f) {
            alpha = 1.0f - ((elapsed - fadeStart) / hitMarkerSettings.fadeTime);
        }
        alpha = std::max(0.0f, std::min(1.0f, alpha));
        
        if (alpha <= 0.0f) continue;
        
        
        float w = viewMatrix.m14 * marker.hitPos.x + viewMatrix.m24 * marker.hitPos.y + 
                  viewMatrix.m34 * marker.hitPos.z + viewMatrix.m44;
        
        if (w <= 0.001f) continue;
        
        
        Vector3 screenPos = FastWorldToScreen(marker.hitPos, viewMatrix);
        
        if (screenPos.z <= 0.001f) continue;
        
        
        const float* color;
        if (hitMarkerSettings.useHitTypeColors) {
            
            if (marker.isKill) {
                color = hitMarkerSettings.killColor;
            } else if (marker.isHeadshot) {
                color = hitMarkerSettings.headshotColor;
            } else {
                color = hitMarkerSettings.hitColor;
            }
        } else {
            
            color = hitMarkerSettings.hitColor;
        }
        
        
        
        std::string displayText = renderDamage ? std::to_string(marker.damage) : "\xEE\xA4\x80";
        
        
        ImFont* font = renderDamage ? sans : hitmarker;
        
        
        float minFontSize = renderDamage ? DAMAGE_MIN_FONT_SIZE : HITMARKER_MIN_FONT_SIZE;
        float fontSize = functions.fontSize;
        if (fontSize < minFontSize) fontSize = minFontSize;
        
        
        ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, displayText.c_str());
        
        
        ImVec2 textPos(screenPos.x - textSize.x / 2.0f, screenPos.y - textSize.y / 2.0f);
        
        
        ImU32 textColor = IM_COL32(
            (int)(color[0] * 255),
            (int)(color[1] * 255),
            (int)(color[2] * 255),
            (int)(color[3] * 255 * alpha)
        );
        
        
        drawList->AddText(font, fontSize, textPos, textColor, displayText.c_str());
    }
}

void HitMarkerSystem::RenderMarkers(const Matrix& viewMatrix) {
    if (!hitMarkerSettings.enabled) return;
    if (hitMarkerSettings.markerType != 0) return; 
    
    std::lock_guard<std::mutex> lock(markersMutex);
    RenderMarkersInternal(markers, viewMatrix, false);
}

void HitMarkerSystem::RenderDamage(const Matrix& viewMatrix) {
    if (!hitMarkerSettings.enabled) return;
    if (hitMarkerSettings.markerType != 1) return; 
    
    std::lock_guard<std::mutex> lock(markersMutex);
    RenderMarkersInternal(markers, viewMatrix, true);
}

void HitMarkerSystem::Clear() {
    std::lock_guard<std::mutex> lock(markersMutex);
    markers.clear();
}
