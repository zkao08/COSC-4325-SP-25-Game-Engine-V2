#include "ViewportWindow.h"

#include <iostream>

ImVec2 ViewportWindow::size;
ImVec2 ViewportWindow::position;
ImVec2 ViewportWindow::regionAvail;

static bool isFocused = false;
static bool isHovered = false;

int ViewportWindow::Render(Renderer* renderer, float scale, ImTextureID texture) {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);

    ImGui::SetWindowFontScale(scale);

    size = ImGui::GetCurrentWindow()->Size;

    ImVec2 windowPos = ImGui::GetWindowPos();

    ImGui::GetWindowDrawList()->AddImage(texture, windowPos, windowPos + ImVec2(2048, 2048));

    renderer->CreateImageButton("SelectTool", "../../../assets/Select.png", ImVec2(30, 30));
    renderer->CreateImageButton("MoveTool", "../../../assets/Move.png", ImVec2(30, 30));
    renderer->CreateImageButton("RotateTool", "../../../assets/Rotate.png", ImVec2(30, 30));
    renderer->CreateImageButton("ResizeTool", "../../../assets/Resize.png", ImVec2(30, 30));

    isFocused = ImGui::IsWindowFocused();
    isHovered = ImGui::IsWindowHovered();

    ImGui::End();

    return 1;
}

ImVec2 ViewportWindow::GetSize() {
    return size;
}

bool ViewportWindow::IsFocused() {
    return isFocused;
}

bool ViewportWindow::IsHovered() {
    return isHovered;
}

/*void ViewportWindow::Clicked(Vector3 camera_pos, int mouse_x, int mouse_y) {
    //std::cout << (mouse_x - position.x) - (camera_pos.x - size.x / 2) << " " << (mouse_y - position.y) - (camera_pos.y - (size.y / 2)) << std::endl;

    std::cout << (mouse_x - camera_pos.x) << " " << (mouse_y - position.y) << std::endl;
}*/