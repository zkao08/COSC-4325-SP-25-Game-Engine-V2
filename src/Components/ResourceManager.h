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
        int width, int height);

    ID3D11ShaderResourceView* GetTexture() const { return m_Texture.Get(); }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    int m_Width;
    int m_Height;
};

// ResourceManager class as a singleton
class ResourceManager
{
public:
    // Singleton accessor
    static ResourceManager& GetInstance();

    // Delete copy and move constructors/assignments
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Initialization
    bool startUp(Renderer* renderer);
    void shutDown();

    // Texture methods
    std::shared_ptr<TextureResource> LoadTexture(const std::string& id, const std::string& filePath);
    std::shared_ptr<TextureResource> GetTexture(const std::string& id);
    void UnloadTexture(const std::string& id);
    void UnloadUnusedTextures(int maxAgeInSeconds = 60);

    // Sound methods - now using SoundResource directly
    std::shared_ptr<SoundResource> LoadSound(const std::string& id, const std::string& filePath,
        bool isStreaming = false, bool loop = true);
    std::shared_ptr<SoundResource> GetSound(const std::string& id);
    void UnloadSound(const std::string& id);
    void UnloadUnusedSounds(int maxAgeInSeconds = 60);

    // General resource management
    void UnloadAllResources();

    // Cache configuration
    void ConfigureTextureCache(size_t maxTextures, size_t minAgeSeconds = 10);
    void ConfigureSoundCache(size_t maxSounds, size_t minAgeSeconds = 10);

    // Regular cleanup methods
    void PerformMaintenance();

private:
    // Private constructor for singleton
    ResourceManager();

    Renderer* m_Renderer;

    // Resource caches
    std::unordered_map<std::string, std::shared_ptr<TextureResource>> m_TextureResources;
    std::unordered_map<std::string, std::shared_ptr<SoundResource>> m_SoundResources;

    // Usage tracking for sounds
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_SoundUsageTimestamps;

    // Cache settings for textures
    size_t m_MaxCachedTextures = 128;
    size_t m_MinTextureAgeSeconds = 10;

    // Cache settings for sounds
    size_t m_MaxCachedSounds = 128;
    size_t m_MinSoundAgeSeconds = 10;

    // Thread safety
    std::mutex m_TextureMutex;
    std::mutex m_SoundMutex;

    // Helper methods for cache cleanup
    void CleanupTextureCache();
    void CleanupSoundCache();
};