#include "includes/tracer.h"
#include "includes/imgui/imgui/imgui.h"
#include <algorithm>

void BulletTracer::AddTrace(const Vector3& start, const Vector3& hit) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(tracesMutex);
    
    BulletTrace trace;
    trace.startPos = start;
    trace.hitPos = hit;
    trace.enemyHitPos = Vector3::Zero();
    trace.timestamp = std::chrono::steady_clock::now();
    trace.duration = displayTime;
    trace.isActive = true;
    trace.hitEnemy = false;
    
    traces.push_back(trace);
    
    
    while (traces.size() > static_cast<size_t>(maxTraces)) {
        traces.erase(traces.begin());
    }
}

void BulletTracer::UpdateLastTraceWithEnemyHit(const Vector3& enemyHitPos) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(tracesMutex);
    
    if (traces.empty()) return;
    
    
    auto& lastTrace = traces.back();
    lastTrace.enemyHitPos = enemyHitPos;
    lastTrace.hitEnemy = true;
}

void BulletTracer::Update() {
    if (!enabled) return;
    RemoveExpiredTraces();
}

void BulletTracer::RemoveExpiredTraces() {
    std::lock_guard<std::mutex> lock(tracesMutex);
    
    auto now = std::chrono::steady_clock::now();
    
    traces.erase(
        std::remove_if(traces.begin(), traces.end(),
            [&now](const BulletTrace& trace) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - trace.timestamp).count() / 1000.0f;
                return elapsed > trace.duration;
            }),
        traces.end()
    );
}

void BulletTracer::Render(const Matrix& viewMatrix) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(tracesMutex);
    
    if (traces.empty()) return;
    
    auto now = std::chrono::steady_clock::now();
    auto drawList = ImGui::GetBackgroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    
    for (const auto& trace : traces) {
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - trace.timestamp).count() / 1000.0f;
        
        
        float alpha = 1.0f;
        float fadeStart = trace.duration - fadeTime;
        if (elapsed > fadeStart && fadeTime > 0.0f) {
            alpha = 1.0f - ((elapsed - fadeStart) / fadeTime);
        }
        alpha = std::max(0.0f, std::min(1.0f, alpha));
        
        if (alpha <= 0.0f) continue;
        
        
        Vector3 endPos = trace.hitPos;
        if (stopOnHit && trace.hitEnemy && trace.enemyHitPos != Vector3::Zero()) {
            endPos = trace.enemyHitPos;
        }
        
        
        float w1 = viewMatrix.m14 * trace.startPos.x + viewMatrix.m24 * trace.startPos.y + 
                   viewMatrix.m34 * trace.startPos.z + viewMatrix.m44;
        float w2 = viewMatrix.m14 * endPos.x + viewMatrix.m24 * endPos.y + 
                   viewMatrix.m34 * endPos.z + viewMatrix.m44;
        
        
        if (w1 <= 0.001f && w2 <= 0.001f) continue;
        
        
        if (w2 <= 0.001f) continue;
        
        Vector3 clippedStart = trace.startPos;
        Vector3 clippedEnd = endPos;
        
        
        if (w1 <= 0.001f) {
            
            
            float nearPlane = 0.01f;
            float t = (nearPlane - w1) / (w2 - w1);
            t = std::max(0.001f, std::min(0.999f, t));
            
            
            clippedStart.x = trace.startPos.x + (endPos.x - trace.startPos.x) * t;
            clippedStart.y = trace.startPos.y + (endPos.y - trace.startPos.y) * t;
            clippedStart.z = trace.startPos.z + (endPos.z - trace.startPos.z) * t;
        }
        
        
        Vector3 screenStart = FastWorldToScreen(clippedStart, viewMatrix);
        Vector3 screenEnd = FastWorldToScreen(clippedEnd, viewMatrix);
        
        
        if (screenStart.z <= 0.001f || screenEnd.z <= 0.001f) continue;
        
        ImVec2 p1(screenStart.x, screenStart.y);
        ImVec2 p2(screenEnd.x, screenEnd.y);
        
        if (useGradient) {
            
            const int segments = 20;
            for (int i = 0; i < segments; i++) {
                float t1 = (float)i / segments;
                float t2 = (float)(i + 1) / segments;
                
                
                ImVec2 segP1(
                    p1.x + (p2.x - p1.x) * t1,
                    p1.y + (p2.y - p1.y) * t1
                );
                ImVec2 segP2(
                    p1.x + (p2.x - p1.x) * t2,
                    p1.y + (p2.y - p1.y) * t2
                );
                
                
                float tMid = (t1 + t2) / 2.0f;
                ImU32 segmentColor = IM_COL32(
                    (int)((color1[0] + (color2[0] - color1[0]) * tMid) * 255),
                    (int)((color1[1] + (color2[1] - color1[1]) * tMid) * 255),
                    (int)((color1[2] + (color2[2] - color1[2]) * tMid) * 255),
                    (int)((color1[3] + (color2[3] - color1[3]) * tMid) * 255 * alpha)
                );
                
                drawList->AddLine(segP1, segP2, segmentColor, lineThickness);
            }
        } else {
            
            ImU32 lineColor = IM_COL32(
                (int)(color1[0] * 255),
                (int)(color1[1] * 255),
                (int)(color1[2] * 255),
                (int)(color1[3] * 255 * alpha)
            );
            
            drawList->AddLine(p1, p2, lineColor, lineThickness);
        }
        
        
        if (showHitPoint) {
            ImU32 hitColor = IM_COL32(
                (int)(hitPointColor[0] * 255),
                (int)(hitPointColor[1] * 255),
                (int)(hitPointColor[2] * 255),
                (int)(hitPointColor[3] * 255 * alpha)
            );
            
            drawList->AddCircleFilled(p2, hitPointRadius, hitColor);
        }
    }
}

void BulletTracer::Clear() {
    std::lock_guard<std::mutex> lock(tracesMutex);
    traces.clear();
}
