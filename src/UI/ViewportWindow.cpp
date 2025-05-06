// ViewportWindow GUI
// Creates and renders the Viewport subwindow.

#include "ViewportWindow.h"

ImVec2 ViewportWindow::size;
ImVec2 ViewportWindow::position;
ImVec2 ViewportWindow::regionAvail;

static bool isFocused = false;
static bool isHovered = false;

// Renders GUI
int ViewportWindow::Render(Renderer* renderer, float scale, ImTextureID texture, Camera* camera) {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);

    ImGui::SetWindowFontScale(scale);

    size = ImGui::GetCurrentWindow()->Size;

    ImVec2 windowPos = ImGui::GetWindowPos();
    Vector3 cameraPos = camera->GetPosition();

    // Loads rendered scene image into viewport window
    ImGui::GetWindowDrawList()->AddImage(texture, windowPos, windowPos + ImVec2(2048, 2048));

    // Camera information
    ImGui::Text(StringToChar("Camera: X[" + RoundString(std::to_string(cameraPos.x), 2) + "] Y[" + RoundString(std::to_string(cameraPos.y), 2) + "]").get());
    ImGui::Text(StringToChar("Zoom: [" + RoundString(std::to_string(cameraPos.z), 1) + "]").get());

    isFocused = ImGui::IsWindowFocused();
    isHovered = ImGui::IsWindowHovered();

    ImGui::End();

    return 1;
}

// Gets the subwindow's size
ImVec2 ViewportWindow::GetSize() {
    return size;
}

// Returns whether the viewport is focused by the user
bool ViewportWindow::IsFocused() {
    return isFocused;
}

// Returns whether viewport is hovered by the user
bool ViewportWindow::IsHovered() {
    return isHovered;
}