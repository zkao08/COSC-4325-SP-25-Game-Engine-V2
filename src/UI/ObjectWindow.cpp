// ObjectWindow GUI
// Creates and renders object creation popup subwindow.

#include "ObjectWindow.h"
#include "Utility.h"

// Window visibility state
static bool enabled = false;

// Preset properties for specific types of objects
// Makes objects distinct and give them certain functionality.
const std::map<std::string, std::map<std::string, PropertyData>> classList{
    {"Sprite_Object that renders a texture.",
        {
            {"Name", {"char", "Sprite"}},
            {"Type", {"const_char", "Object"}},
            {"Parent", {"const_char", "null"}},
            {"Position", {"Vector2", "0,0"}},
            {"Rotation", {"float", "0.0"}},
            {"Size", {"Vector2", "1,1"}},
            {"Texture", {"string", GetProjectRoot() + "\\assets\\Square.png"}},
            {"Static", {"bool", "true"}},
            {"Collidable", {"bool", "true"}},
            {"Upright", {"bool", "false"}},
        }
    },
    {"Folder_Contains objects.",
        {
            {"Name", {"char", "Folder"}},
            {"Type", {"const_char", "Folder"}},
            {"Parent", {"const_char", "null"}}
        }
    },
    {"Script_Runs Lua code.",
        {
            {"Name", {"char", "Script"}},
            {"Type", {"const_char", "Script"}},
            {"Parent", {"const_char", "null"}},
            {"Script", {"string", ""}},
            {"Enabled", {"bool", "true"}},
        }
    },
};

struct ClassInfo {
    std::string name;
    std::string description;
};

ClassInfo ParseText(std::string);

// Render GUI
void ObjectWindow::Render(Renderer* renderer, Game* game, float scale) {
    if (enabled) {
        char* searchText = "";

        ImGuiWindowClass noDockWindow;
        noDockWindow.ClassId = ImGui::GetID("NoDockWindow");
        noDockWindow.DockingAllowUnclassed = 0;
        ImGui::SetNextWindowClass(&noDockWindow);

        ImGui::Begin("Add Object", &enabled);

        ImGui::SetWindowFontScale(scale);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##", "Search...", searchText, 100);

        ImGui::Separator();

        for (std::pair item : classList) {
            ClassInfo info = ParseText(item.first);
            ImGui::BeginGroup();
            if (ImGui::ButtonEx(info.name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
                Object* obj = new Object(renderer, info.name, item.second, game->devMode);

                if (game->GetSelectedObjects().size() > 0)
                    game->GetSelectedObjects()[0]->AddChild(obj);
                else
                    game->GetGameObject()->AddChild(obj);

                game->SelectObject(obj);

                enabled = false;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(info.description.c_str());
            }
            ImGui::EndGroup();
        }

        ImGui::End();
    }
}

// Separates property map key into name and description
ClassInfo ParseText(std::string str) {
    ClassInfo info;
    std::string buffer;

    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '_') {
            info.name = buffer;
            buffer = "";
        }
        else
            buffer += str[i];
    }

    info.description = buffer;

    return info;
}

// Changes window visibility
void ObjectWindow::Toggle(bool toggle) {
    enabled = toggle;
}

// Toggles window visibility
void ObjectWindow::Toggle() {
    enabled = !enabled;
}