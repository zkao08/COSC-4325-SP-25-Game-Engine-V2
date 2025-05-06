// MainMenuBar GUI
// Creates and renders the top menu bar in the level editor.

#include "MainMenuBar.h"

int MainMenuBar::Render() {
    MainMenuBarResult result = NONE;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                result = CLOSE_APP;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("Run Game"))
                result = RUN_GAME;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    return result;
}