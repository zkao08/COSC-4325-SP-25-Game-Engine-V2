#include "MainMenuBar.h"

int MainMenuBar::Render() {
    MainMenuBarResult result = NONE;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New (TODO)", "Ctrl+N");
            ImGui::MenuItem("Load... (TODO)");
            ImGui::MenuItem("Save (TODO)", "Ctrl+S");
            ImGui::MenuItem("Save As... (TODO)", "Ctrl+Shift+S");
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                result = CLOSE_APP;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("TODO");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("TODO");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("Run Game"))
                result = RUN_GAME;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("TODO");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    return result;
}