#pragma once

#include <string>
#include <memory>
#include "Vector2.h"

namespace {
    std::unique_ptr<char> StringToChar(std::string str) {
        std::unique_ptr<char> charStr(new char[128]);

        strcpy(charStr.get(), str.c_str());

        return charStr;
    }

    Vector2 StringToVector2(std::string text) {
        Vector2 vec2;
        std::string buffer;
        bool readY = true;

        for (int i = 0; i < text.length(); i++) {
            if (text[i] == ',' && buffer != "") {
                vec2.x = std::stof(buffer);
                buffer = "";
            }
            else
                buffer += text[i];
        }

        if (buffer != "")
            vec2.y = std::stof(buffer);

        return vec2;
    }

    std::string Vector2ToString(Vector2 vec2) {
        return std::to_string(vec2.x) + ',' + std::to_string(vec2.y);
    }
}