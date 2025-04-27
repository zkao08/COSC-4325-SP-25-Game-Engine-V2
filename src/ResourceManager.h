// ResourceManager.h
#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <chrono>
#include <d3d11.h>
#include <wrl/client.h>
#include "PathUtils.h"

// Forward declarations
class SoundResource;
class Renderer;

// Base Resource class
class Resource
{
public:
    Resource(const std::string& id) : m_Id(id) {}
    virtual ~Resource() = default;

    const std::string& GetId() const { return m_Id; }

    // Track when this resource was last used (for cache management)
    void MarkAsUsed() { m_LastUsedTime = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point GetLastUsedTime() const { return m_LastUsedTime; }

private:
    std::string m_Id;
    std::chrono::steady_clock::time_point m_LastUsedTime = std::chrono::steady_clock::now();
};

// Texture Resource
class TextureResource : public Resource
{
public:
    TextureResource(const std::string& id, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
        int width, int height)
        : Resource(id), m_Texture(texture), m_Width(width), m_Height(height) {
    }

    ID3D11ShaderResourceView* GetTexture() const { return m_Texture.Get(); }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    int m_Width;
    int m_Height;
};

// Sound Resource
class SoundResourceWrapper : public Resource
{
public:
    SoundResourceWrapper(const std::string& id, std::shared_ptr<SoundResource> sound)
        : Resource(id), m_Sound(sound) {
    }

    std::shared_ptr<SoundResource> GetSound() const { return m_Sound; }

private:
    std::shared_ptr<SoundResource> m_Sound;
};

// ResourceManager class as a singleton
class ResourceManager
{
public:
    // Singleton accessor
    static ResourceManager& GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    // Delete copy and move constructors/assignments
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Initialize with renderer
    bool Initialize(Renderer* renderer);
    void Shutdown();

    // Texture methods
    std::shared_ptr<TextureResource> LoadTexture(const std::string& id, const std::string& filePath);
    std::shared_ptr<TextureResource> GetTexture(const std::string& id);

    // Sound methods
    std::shared_ptr<SoundResourceWrapper> LoadSound(const std::string& id, const std::string& filePath, bool isStreaming = false);
    std::shared_ptr<SoundResourceWrapper> GetSound(const std::string& id);

    // Resource management
    void UnloadResource(const std::string& id);
    void UnloadUnusedResources(int maxAgeInSeconds = 60);
    void UnloadAllResources();

    // Cache configuration
    void ConfigureCache(size_t maxCachedResources, size_t minResourceAgeSeconds = 10);

private:
    // Private constructor for singleton
    ResourceManager() : m_Renderer(nullptr) {}

    Renderer* m_Renderer;
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_Resources;

    // Cache settings
    size_t m_MaxCachedResources = 100;
    size_t m_MinResourceAgeSeconds = 10;

    // Thread safety
    std::mutex m_ResourceMutex;

    // Helper methods
    void CleanupResourceCache();
};