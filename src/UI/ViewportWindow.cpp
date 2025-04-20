#include "ViewportWindow.h"

ImVec2 ViewportWindow::size;

static bool isFocused = false;

int ViewportWindow::Render(Renderer* renderer, float scale, ImTextureID texture) {
    ComPtr<ID3D11DeviceContext> context = renderer->GetContext();

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);

    ImGui::SetWindowFontScale(scale);

    size = ImGui::GetCurrentWindow()->Size;

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 topLeft = windowPos;
    ImVec2 bottomRight = ImVec2(windowPos.x + (windowSize.x * 2), windowPos.y + (windowSize.y * 2));

    ImGui::GetWindowDrawList()->AddImage(texture, topLeft, bottomRight);

    renderer->CreateImageButton("SelectTool", "../../../assets/Select.png", ImVec2(30, 30));
    renderer->CreateImageButton("MoveTool", "../../../assets/Move.png", ImVec2(30, 30));
    renderer->CreateImageButton("RotateTool", "../../../assets/Rotate.png", ImVec2(30, 30));
    renderer->CreateImageButton("ResizeTool", "../../../assets/Resize.png", ImVec2(30, 30));

    isFocused = ImGui::IsWindowFocused();

    ImGui::End();

    return 1;
}

ImVec2 ViewportWindow::GetSize() {
    return size;
}

bool ViewportWindow::IsFocused() {
    return isFocused;
}