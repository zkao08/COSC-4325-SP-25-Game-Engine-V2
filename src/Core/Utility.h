#pragma once

#include <Windows.h>

#include <string>
#include <memory>
#include <cmath>

#include "Vector2.h"

namespace {
    std::unique_ptr<char> StringToChar(std::string str) {
        std::unique_ptr<char> charStr(new char[128]);

        strcpy_s(charStr.get(), 128, str.c_str());

        return charStr;
    }

    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
            return std::wstring();

        int str_size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);

        std::wstring conversion(str_size, 0);
        //MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), (LPWSTR)conversion.data(), str_size);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &conversion.data()[0], str_size);
        return conversion;
    }

    std::string WStringToString(const std::wstring& str)
    {
        if (str.empty())
            return std::string();

        int str_size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);

        std::string conversion;
        conversion.resize(str_size);

        WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), (LPSTR)conversion.data(), str_size, NULL, NULL);
        return conversion;
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

        if (buffer == "" || buffer == "-")
            return "0";

        return buffer;
    }

    void GetResolution(int& x, int& y)
    {
        RECT desktop;

        const HWND hDesktop = GetDesktopWindow();

        GetWindowRect(hDesktop, &desktop);

        x = desktop.right;
        y = desktop.bottom;
    }

    std::string GetProjectRoot()
    {
        // Get current working directory
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);

        // Remove "\build" from the path if it exists
        std::wstring path(buffer);
        std::wstring buildDir = L"\\build";
        size_t pos = path.find(buildDir);
        if (pos != std::wstring::npos) {
            path = path.substr(0, pos);
        }

        std::string strPath(path.begin(), path.end());

        return strPath;
    }

    std::wstring GetProjectRootWString()
    {
        // Get current working directory
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);

        // Remove "\build" from the path if it exists
        std::wstring path(buffer);
        std::wstring buildDir = L"\\build";
        size_t pos = path.find(buildDir);
        if (pos != std::wstring::npos) {
            path = path.substr(0, pos);
        }

        return path;
    }
}