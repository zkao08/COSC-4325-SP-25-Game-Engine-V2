#include "ObjectWindow.h"

#include <iostream>

static bool enabled = false;

const std::map<std::string, std::map<std::string, PropertyData>> classList{
            {"Object_Base class.",
                {
                    {"Name", {"char", "Object"}},
                    {"Position", {"Vector2", "0,0"}},
                    {"Rotation", {"float", "0.0"}},
                    {"Size", {"Vector2", "0,0"}}
                }
            },
            {"Folder_Contains objects.",
                {
                    {"Name", {"char", "Folder"}},
                }
            },
};

struct ClassInfo {
    std::string name;
    std::string description;
};

ClassInfo ParseText(std::string);

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
                //std::unique_ptr<Object> obj = std::make_unique<Object>(renderer, info.name);
                Object* obj = new Object(renderer, info.name, item.second);
                game->AddObject(obj);
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

void ObjectWindow::Toggle(bool toggle) {
    enabled = toggle;
}

void ObjectWindow::Toggle() {
    enabled = !enabled;
} 