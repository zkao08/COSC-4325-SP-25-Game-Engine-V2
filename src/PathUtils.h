#pragma once

#include <string>
#include <Windows.h>
#include <filesystem>

class PathUtils
{
public:
    /**
     * Gets the absolute path to the project root directory from any location
     * @return Wide string containing the project root path
     */
    static std::wstring GetProjectRoot()
    {
        // Get current working directory
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);
        
        // Convert to filesystem path for easier manipulation
        std::filesystem::path currentPath(buffer);
        
        // Search for common build directory names
        while (!currentPath.empty() && 
               currentPath.has_filename() && 
               currentPath.has_parent_path())
        {
            // Check if current directory is a build directory
            std::wstring dirName = currentPath.filename().wstring();
            if (dirName == L"build" || dirName == L"bin" || dirName == L"out" || 
                dirName == L"Debug" || dirName == L"Release")
            {
                // Return parent of build directory as project root
                return currentPath.parent_path().wstring();
            }
            
            // If we find src or source directory, we're likely in the project structure
            if (dirName == L"src" || dirName == L"source")
            {
                // Return parent of src directory as project root
                return currentPath.parent_path().wstring();
            }
            
            // Move up one directory
            currentPath = currentPath.parent_path();
        }
        
        // If no build directory was found, return the original path
        // This handles the case where the executable is already at the root
        return std::wstring(buffer);
    }
    
    /**
     * Gets an absolute file path by combining the project root with a relative path
     * @param relativePath Relative path from project root to the file
     * @return Wide string containing the absolute file path
     */
    static std::wstring GetAbsolutePath(const std::wstring& relativePath)
    {
        std::wstring root = GetProjectRoot();

        // Check if the path is already absolute (starts with a drive letter)
        if (relativePath.length() >= 2 && relativePath[1] == L':') {
            return relativePath; // Already an absolute path
        }

        // Normalize path separators
        std::wstring normalizedPath = relativePath;
        std::replace(normalizedPath.begin(), normalizedPath.end(), L'/', L'\\');

        // Remove leading backslash if present
        if (!normalizedPath.empty() && normalizedPath[0] == L'\\') {
            normalizedPath = normalizedPath.substr(1);
        }

        // Ensure root path ends with backslash
        if (!root.empty() && root.back() != L'\\') {
            root += L'\\';
        }

        return root + normalizedPath;
    }
    
    /**
     * Gets an absolute file path by combining the project root with a relative path
     * @param relativePath Relative path from project root to the file
     * @return String containing the absolute file path
     */
    static std::string GetAbsolutePath(const std::string& relativePath)
    {
        // Convert to wide string, process, then convert back
        std::wstring widePath = StringToWString(relativePath);
        std::wstring result = GetAbsolutePath(widePath);
        return WStringToString(result);
    }
    
    /**
     * Converts a standard string to a wide string
     * @param str String to convert
     * @return Wide string version
     */
    static std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
        {
            return std::wstring();
        }
        
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
        
        return wstr;
    }
    
    /**
     * Converts a wide string to a standard string
     * @param wstr Wide string to convert
     * @return Standard string version
     */
    static std::string WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty())
        {
            return std::string();
        }
        
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
        
        return str;
    }
};