#include "NavigatorWindow.h"

int NavigatorWindow::Render(Renderer* renderer, Game* game, float scale) {
    char* searchText = "";

    ImGui::Begin("Navigator");

    ImGui::SetWindowFontScale(scale);

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        ImGui::OpenPopup("Context Menu");
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        game->DeselectObjects();
    }

    if (ImGui::BeginPopup("Context Menu")) {
        if (ImGui::MenuItem("Add Item...")) {
            EntityWindow::ToggleEntityWindow(true);
        }
        ImGui::EndPopup();
    }

    if (renderer->CreateImageButton("Refresh", "../../../assets/Refresh.png", ImVec2(24, 24))) {
        // No implementation yet.
    }

    ImGui::SameLine();

    if (renderer->CreateImageButton("Add", "../../../assets/Add.png", ImVec2(24, 24)))
        EntityWindow::ToggleEntityWindow();

    ImGui::Separator();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint(" ", "Search...", searchText, 100);

    ImGui::Separator();

    for (int i = 0; i < game->GetObjects().size(); i++) {
        GenerateItemTreeNodes(game->GetObjects()[i], game);
    }

    ImGui::End();

    return 1;
}

void NavigatorWindow::GenerateItemTreeNodes(Entity* item, Game* game) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    for (int i = 0; i < game->GetSelectedObjects().size(); i++) {
        if (game->IsObjectSelected(StringToChar(item->properties["Name"].Data).get()))
            flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (item->children.size() == 0)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool open = ImGui::TreeNodeEx(StringToChar(item->properties["Name"].Data).get(), flags);
    bool clicked = ImGui::IsItemClicked();

    if (open) {
        for (int i = 0; i < item->children.size(); i++) {
            GenerateItemTreeNodes(item->children[i], game);
        }
        ImGui::TreePop();
    }
    if (clicked) {
        game->DeselectObjects();
        game->SelectObject(item);
    }
}