#pragma once

#include <string>
#include <memory>
#include <cmath>

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

    float RoundFloat(float num, float precision = 0.001) {
        return std::round((num / precision) * precision);
    }

    std::string RoundString(std::string str, int place = 3) {
        std::string buffer;
        int currentPlace = 0;
        int decimalFound = false;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == '.')
                decimalFound = true;
            else if (decimalFound) {
                currentPlace++;
            }
            if (currentPlace > place)
                break;
            
            buffer += str[i];
        }

        return buffer;
    }
}