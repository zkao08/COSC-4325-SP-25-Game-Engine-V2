#include "EntityWindow.h"

EntityWindow::EntityWindow() {
    enabled = false;
}

void EntityWindow::RenderEntityWindow(float scale) {
    if (enabled) {
        char* searchText = "";

        ImGuiWindowClass noDockWindow;
        noDockWindow.ClassId = ImGui::GetID("NoDockWindow");
        noDockWindow.DockingAllowUnclassed = 0;
        ImGui::SetNextWindowClass(&noDockWindow);

        ImGui::Begin("Add Entity", &enabled);

        ImGui::SetWindowFontScale(scale);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##", "Search...", searchText, 100);

        ImGui::Separator();

        for (auto const& v : classes) {
            ImGui::BeginGroup();
            ImGui::EndGroup();
        }

        ImGui::End();
    }
}

void EntityWindow::ToggleEntityWindow(bool toggle) {
    enabled = toggle;
}

void EntityWindow::ToggleEntityWindow() {
    enabled = !enabled;
}