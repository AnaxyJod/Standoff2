#include "../includes/uses.h"
#include "../includes/players.h"
#include "../includes/fonts.h"
#include "../menu/ui.h"


uint64_t GetPhotonPlayer(uint64_t player) {
    if (!player || player < 0x10000) return 0;
    return mem.read<uint64_t>(player + offsets::photonPointer);
}


uint64_t GetRoom() {
    if (!libUnity.start) return 0;
    
    
    uint64_t pm1 = mem.read<uint64_t>(libUnity.start + offsets::PhotonNetwork_addr);
    if (!pm1 || pm1 < 0x10000) return 0;
    
    uint64_t staticFields = mem.read<uint64_t>(pm1 + offsets::manager_ptr2());
    if (!staticFields || staticFields < 0x10000) return 0;
    
    uint64_t networkingPeer = mem.read<uint64_t>(staticFields + offsets::photonnetwork_ptr1());
    if (!networkingPeer || networkingPeer < 0x10000) return 0;
    
    uint64_t room = mem.read<uint64_t>(networkingPeer + offsets::photon_room());
    if (!room || room < 0x10000) return 0;
    
    return room;
}


bool IsHost(uint64_t photonPlayer) {
    if (!photonPlayer || photonPlayer < 0x10000) return false;
    
    uint64_t room = GetRoom();
    if (!room) return false;
    
    
    int32_t masterClientId = mem.read<int32_t>(room + offsets::roomMasterActorNumber);
    
    
    int32_t actorId = mem.read<int32_t>(photonPlayer + offsets::photonActorNumber);
    
    return masterClientId == actorId;
}


bool IsPlayerHost(uint64_t player) {
    if (!player || player < 0x10000) return false;
    
    uint64_t photonPlayer = GetPhotonPlayer(player);
    return IsHost(photonPlayer);
}


void HostIndicator() {
    
    if (!menu.show_host_indicator) return;
    if (!libUnity.start) return;
    
    
    uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
    if (!playerManager || playerManager < 0x10000) {
        playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, false, 0x0);
    }
    if (!playerManager || playerManager < 0x10000) return;
    
    uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
    if (!localPlayer || localPlayer < 0x10000) return;
    
    
    uint64_t photonPlayer = GetPhotonPlayer(localPlayer);
    bool isHost = IsHost(photonPlayer);
    
    
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImGuiIO& io = ImGui::GetIO();
    
    const char* text = "HOST";
    
    ImU32 color = isHost ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
    
    
    float fontSize = 30.0f;
    ImVec2 textPos = ImVec2(10.0f, io.DisplaySize.y / 2.0f);
    
    
    ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx != 0 || dy != 0) {
                drawList->AddText(font_pixel, fontSize, ImVec2(textPos.x + dx, textPos.y + dy), outlineColor, text);
            }
        }
    }
    
    
    drawList->AddText(font_pixel, fontSize, textPos, color, text);
}
