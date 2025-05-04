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
            float width = ImGui::GetContentRegionAvail().x / 2.05f;
            std::string buffer;
            bool conversionSuccess = true;

            buffer = RoundString(std::to_string(vec2.x));
            strcpy_s(xText, 16, buffer.c_str());
            buffer = RoundString(std::to_string(vec2.y));
            strcpy_s(yText, 16, buffer.c_str());

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

            strcpy_s(text, 8, RoundString(property.second.Data, 2).c_str());

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputText(("##" + property.first).c_str(), text, 8, ImGuiInputTextFlags_CharsDecimal)) {
                property.second.Data = text;
            }

            delete[] text;
        }
        else if (property.second.DataType == "bool" || property.second.DataType == "boolean") {
            bool checked;

            if (property.second.Data == "true")
                checked = true;
            else
                checked = false;

            if (ImGui::Checkbox(("##" + property.first).c_str(), &checked)) {
                if (checked)
                    property.second.Data = "true";
                else
                    property.second.Data = "false";
            }
        }
        else if (property.second.DataType == "const_Vector2") {
            Vector2 vec2 = StringToVector2(property.second.Data);
            float width = ImGui::GetContentRegionAvail().x / 2.05f;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1));

            ImGui::SetNextItemWidth(width);
            ImGui::InputText(("##" + property.first + "X").c_str(), StringToChar(RoundString(std::to_string(vec2.x))).get(), 16, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsDecimal);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(width);
            ImGui::InputText(("##" + property.first + "Y").c_str(), StringToChar(RoundString(std::to_string(vec2.y))).get(), 16, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsDecimal);

            ImGui::PopStyleColor(1);
        }
        else if (property.second.DataType == "const_float") {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText(("##" + property.first).c_str(), StringToChar(RoundString(property.second.Data, 2)).get(), 8, ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopStyleColor(1);
        }
        else if (property.second.DataType == "const_char") {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText(("##" + property.first).c_str(), &property.second.Data, ImGuiInputTextFlags_ReadOnly, nullptr, nullptr);
            ImGui::PopStyleColor(1);
        }
        else { // char
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText(("##" + property.first).c_str(), &property.second.Data, ImGuiInputTextFlags_None, nullptr, nullptr);
        }
    }
}