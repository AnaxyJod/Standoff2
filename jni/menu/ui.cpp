#include "ui.h"
#include "../includes/uses.h"
#include "../includes/imgui/imgui/imgui.h"
#include "../includes/aim.h"
#include "../includes/rcs.h"

void g_menu::render() {
    if (!ImGui::GetCurrentContext() || !draw::isFrameReady()) {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        functions.show_menu = !functions.show_menu;
    }

    if (!functions.show_menu) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(420, 340), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("zov (safe mode)", nullptr, flags)) {
        ImGui::Text("Minimal stable menu");
        ImGui::Separator();

        ImGui::Checkbox("ESP", &functions.esp.enabled);
        ImGui::Checkbox("Aimbot", &aimSettings.enabled);
        ImGui::Checkbox("RCS", &recoilSettings.enabled);
        ImGui::Checkbox("No Recoil", &functions.noRecoil);

        ImGui::Separator();

        ImGui::Checkbox("Infinity Ammo", &functions.infinityAmmo);
        ImGui::Checkbox("Money Hack", &functions.moneyHack);
        ImGui::Checkbox("Wallshot", &functions.wallshot);
        ImGui::Checkbox("Fire Rate", &functions.fireRate);

        ImGui::Separator();

        ImGui::Text("Players: %d", functions.playerCount);
        ImGui::Text("Alive: %s", functions.isAlive ? "yes" : "no");

        if (ImGui::Button("Exit")) {
            should_exit = true;
        }
    }
    ImGui::End();
}
