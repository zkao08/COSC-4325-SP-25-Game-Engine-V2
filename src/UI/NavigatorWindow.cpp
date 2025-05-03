#include "NavigatorWindow.h"

static int idCount = 0;

int NavigatorWindow::Render(Renderer* renderer, Game* game, float scale) {
    char* searchText = "";
    static bool rightClicked = false;

    ImGui::Begin("Navigator");

    ImGui::SetWindowFontScale(scale);

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        rightClicked = true;
        ImGui::OpenPopup("Context Menu");
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        rightClicked = false;
        game->DeselectObjects();
    }

    if (renderer->CreateImageButton("Refresh", "../../../assets/Refresh.png", ImVec2(24, 24))) {
        // No implementation yet.
    }

    ImGui::SameLine();

    if (renderer->CreateImageButton("Add", "../../../assets/Add.png", ImVec2(24, 24)))
        ObjectWindow::Toggle();

    ImGui::Separator();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint(" ", "Search...", searchText, 100);

    ImGui::Separator();

    for (int i = 0; i < game->GetObjects().size(); i++) {
        if (!game->GetObjects()[i]->markedDeleted)
            GenerateItemTreeNodes(game->GetObjects()[i], game);
    }

    if (rightClicked && game->GetSelectedObjects().size() != 0) {
        if (ImGui::BeginPopup("Context Menu")) {
            if (ImGui::MenuItem("Add Item...")) {
                ObjectWindow::Toggle(true);
            }
            if (ImGui::MenuItem("Delete")) {
                game->DeleteObject(game->GetSelectedObjects()[0]->properties["Name"].Data);
                game->DeselectObjects();
            }
            ImGui::EndPopup();
        }
    }
    else if (rightClicked) {
        if (ImGui::BeginPopup("Context Menu")) {
            if (ImGui::MenuItem("Add Item...")) {
                ObjectWindow::Toggle(true);
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();

    return 1;
}

void NavigatorWindow::GenerateItemTreeNodes(Object* item, Game* game) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    for (int i = 0; i < game->GetSelectedObjects().size(); i++) {
        if (game->IsObjectSelected(item))
            flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (item->children.size() == 0)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool open = ImGui::TreeNodeEx(StringToChar(item->properties["Name"].Data + "##" + std::to_string(idCount++)).get(), flags);
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    bool rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

    if (open) {
        for (int i = 0; i < item->children.size(); i++) {
            GenerateItemTreeNodes(item->children[i], game);
        }
        ImGui::TreePop();
    }
    if (clicked || rightClicked) {
        game->DeselectObjects();
        game->SelectObject(item);
    }
}