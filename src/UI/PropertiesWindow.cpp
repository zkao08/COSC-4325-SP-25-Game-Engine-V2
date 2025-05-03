#include "PropertiesWindow.h"

int PropertiesWindow::Render(Game* game, float scale) {
    char* searchText = "";

    ImGui::Begin("Properties");

    ImGui::SetWindowFontScale(scale);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##SearchBar", "Search...", searchText, 100);

    ImGui::Separator();

    ImGui::SeparatorText("##PropertiesSearchSeparator");
    if (game->GetSelectedObjects().size() > 0)
        LoadProperties(game->GetSelectedObjects()[0]);
    else
        LoadProperties(nullptr);

    ImGui::End();

    return 1;
}

void PropertiesWindow::LoadProperties(Object* obj) {
    if (obj == nullptr) {
        ImGui::Text("No object selected.");
        return;
    }

    for (auto& property : obj->properties) {
        ImGui::Text(property.first.c_str());
        ImGui::SameLine();
        if (property.second.DataType == "Vector2") {
            Vector2 vec2 = StringToVector2(property.second.Data);
            char* xText = new char[16];
            char* yText = new char[16];
            float width = ImGui::GetContentRegionAvail().x / 3.0f;
            std::string buffer;
            bool conversionSuccess = true;

            buffer = RoundString(std::to_string(vec2.x));
            strcpy(xText, buffer.c_str());
            buffer = RoundString(std::to_string(vec2.y));
            strcpy(yText, buffer.c_str());

            ImGui::SetNextItemWidth(width);
            if (!ImGui::InputText(("##" + property.first + "X").c_str(), xText, 16, ImGuiInputTextFlags_CharsDecimal))
                buffer = xText;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(width);
            if (!ImGui::InputText(("##" + property.first + "Y").c_str(), yText, 16, ImGuiInputTextFlags_CharsDecimal))
                buffer = yText;

            try {
                buffer = xText;
                vec2.x = stof(buffer);
                buffer = yText;
                vec2.y = stof(buffer);
            }
            catch (std::invalid_argument e) {
                conversionSuccess = false;
            }

            if (conversionSuccess)
                property.second.Data = Vector2ToString(vec2);

            delete[] xText;
            delete[] yText;
        }
        else if (property.second.DataType == "float") {
            char* text = new char[8];

            strcpy(text, RoundString(property.second.Data, 2).c_str());

            if (ImGui::InputText(("##" + property.first + "Y").c_str(), text, 8, ImGuiInputTextFlags_CharsDecimal)) {
                property.second.Data = text;
            }

            delete[] text;
        }
        else {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText(("##" + property.first).c_str(), &property.second.Data, ImGuiInputTextFlags_None, nullptr, nullptr);
        }
    }
}