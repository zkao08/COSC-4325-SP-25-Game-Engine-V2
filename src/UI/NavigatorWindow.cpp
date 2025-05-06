// NavigatorWindow GUI
// Creates and renders Navigator subwindow.

#include "NavigatorWindow.h"

Object* NavigatorWindow::heldObject;

// File path to the Add (+) icon
const std::string ADD_PATH = GetProjectRoot() + "/assets/Add.png";

// Renders the GUI
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

    ImGui::SameLine();

    if (renderer->CreateImageButton("Add", const_cast<char*>(ADD_PATH.c_str()), ImVec2(24, 24)))
        ObjectWindow::Toggle();

    ImGui::Separator();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint(" ", "Search...", searchText, 100);

    ImGui::Separator();

    for (int i = 0; i < game->GetGameObject()->children->size(); i++) {
        if (!game->GetGameObject()->children->at(i)->markedDeleted)
            GenerateItemTreeNodes(game->GetGameObject()->children->at(i), game, i);
    }

    if (rightClicked && game->GetSelectedObjects().size() != 0 && heldObject == nullptr) {
        if (ImGui::BeginPopup("Context Menu")) {
            if (ImGui::MenuItem("Add Item...")) {
                ObjectWindow::Toggle(true);
            }
            if (ImGui::MenuItem("Duplicate")) {
                Object* clone = new Object(game->GetSelectedObjects()[0], renderer, game->devMode);
                game->GetSelectedObjects()[0]->parent->AddAfterChild(game->GetSelectedObjects()[0], clone);
                game->DeselectObjects();
                game->SelectObject(clone);
            }
            if (ImGui::MenuItem("Delete")) {
                game->GetGameObject()->DeleteChild(game->GetSelectedObjects()[0], true);
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

// Renders the object buttons that exist in the game world
void NavigatorWindow::GenerateItemTreeNodes(Object* item, Game* game, int id) {
    std::string fullId = item->properties["Name"].Data + "##" + std::to_string(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    for (int i = 0; i < game->GetSelectedObjects().size(); i++) {
        if (game->IsObjectSelected(item))
            flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (item->children->size() == 0)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool open = ImGui::TreeNodeEx(StringToChar(fullId).get(), flags);
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    bool rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
    bool middleClicked = ImGui::IsItemClicked(ImGuiMouseButton_Middle);
    bool mouseRightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);

    if (open) {
        for (int i = 0; i < item->children->size(); i++) {
            GenerateItemTreeNodes(item->children->at(i), game, i);
        }
        ImGui::TreePop();
    }
    if (clicked || rightClicked) {
        game->DeselectObjects();
        if (heldObject != nullptr) {
            if (heldObject != item && !heldObject->IsDescendant(item)) {
                heldObject->parent->RemoveChild(heldObject);
                item->AddChild(heldObject);
                game->SelectObject(heldObject);
                heldObject = nullptr;
            }
            else
                heldObject = nullptr;
        }
        else
            game->SelectObject(item);
    }
    if (middleClicked)
        heldObject = item;
    if (heldObject != nullptr && heldObject == item) {
        ImGui::BeginTooltip();
        ImGui::Text(StringToChar(item->properties["Name"].Data).get());
        ImGui::EndTooltip();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (heldObject)
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75, 0.75, 0.75, 1));
    else
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75, 0.75, 0.75, 0));

    if (ImGui::Button(StringToChar("##After_" + fullId).get(), ImVec2(ImGui::GetContentRegionAvail().x, 3)) && heldObject != nullptr) {
        if (heldObject != item && !heldObject->IsDescendant(item)) {
            heldObject->parent->RemoveChild(heldObject);
            item->parent->AddAfterChild(item, heldObject);
            heldObject = nullptr;
        }
        else
            heldObject = nullptr;
    }

    if (mouseRightDown)
        heldObject = nullptr;

    ImGui::PopStyleColor(2);
}