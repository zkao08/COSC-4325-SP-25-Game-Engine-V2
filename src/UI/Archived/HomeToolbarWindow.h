// HomeToolBarWindow UI Header
// Creates Home toolbar subwindow and adds functionality.

#pragma once

#include "imgui.h"
#include "Renderer.h"

int RenderHomeToolbarWindow(Renderer* renderer, float scale) {
    ImGui::Begin("Home");

    int size_x, size_y = 24;

    ImGui::SetWindowFontScale(scale);

    ID3D11ShaderResourceView* placeholderImage = NULL;
    bool result = renderer->LoadTextureFromFile("../../../assets/Placeholder.png", &placeholderImage, &size_x, &size_y); // Note that relative path begins in the build folder, not src.

    if (result) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        for (int i = 0; i < 10; i++) {
            ImGui::BeginGroup();
            ImGui::ImageButton("TestButton_" + char(i), (ImTextureID)(intptr_t)placeholderImage, ImVec2(ImGui::GetContentRegionAvail().y - 8, ImGui::GetContentRegionAvail().y - 8));
            ImGui::EndGroup();
            ImGui::SameLine();
        }
        ImGui::PopStyleColor(3);
    }
    else
        ImGui::Button("TestButton");

    ImGui::End();

    return 1;
}