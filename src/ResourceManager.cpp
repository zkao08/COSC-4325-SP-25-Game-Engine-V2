// ResourceManager.cpp
#include "ResourceManager.h"
#include "Renderer.h"
#include "SoundResource.h"
#include <algorithm>
#include <iostream>

/// <summary>
/// Constructor for TextureResource
/// </summary>
TextureResource::TextureResource(const std::string& id, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
    int width, int height)
    : Resource(id), m_Texture(texture), m_Width(width), m_Height(height)
{
}

/// <summary>
/// Get the singleton instance
/// </summary>
ResourceManager& ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

/// <summary>
/// Private constructor for singleton pattern
/// </summary>
ResourceManager::ResourceManager()
    : m_Renderer(nullptr)
{
}

/// <summary>
/// Initialize the resource manager with a renderer
/// </summary>
bool ResourceManager::Initialize(Renderer* renderer)
{
    m_Renderer = renderer;
    std::cout << "ResourceManager initialized" << std::endl;
    return true;
}

/// <summary>
/// Shutdown the resource manager and unload all resources
/// </summary>
void ResourceManager::Shutdown()
{
    UnloadAllResources();
    std::cout << "ResourceManager shut down" << std::endl;
}

/// <summary>
/// Load a texture from file
/// </summary>
std::shared_ptr<TextureResource> ResourceManager::LoadTexture(const std::string& id, const std::string& filePath)
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(m_TextureMutex);

    // Check if already loaded
    auto it = m_TextureResources.find(id);
    if (it != m_TextureResources.end())
    {
        it->second->MarkAsUsed();
        return it->second;
    }

    // Get absolute path using PathUtils
    std::string absolutePath = PathUtils::GetAbsolutePath(filePath);

    // Load new texture
    ID3D11ShaderResourceView* texturePtr = nullptr;
    int width = 0, height = 0;

    // Use existing renderer method to load texture
    bool result = m_Renderer->LoadTextureFromFile(absolutePath.c_str(), &texturePtr, &width, &height);

    if (!result || !texturePtr)
    {
        std::cerr << "Failed to load texture: " << id << " from path: " << absolutePath << std::endl;
        return nullptr;
    }

    // Create a ComPtr to manage the texture
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
    textureView.Attach(texturePtr);

    // Create texture resource
    auto textureResource = std::make_shared<TextureResource>(id, textureView, width, height);
    m_TextureResources[id] = textureResource;

    std::cout << "Loaded texture: " << id << " (" << width << "x" << height << ")" << std::endl;

    // Check if we need to clean up cache
    CleanupTextureCache();

    return textureResource;
}

/// <summary>
/// Get a texture by ID
/// </summary>
std::shared_ptr<TextureResource> ResourceManager::GetTexture(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);

    auto it = m_TextureResources.find(id);
    if (it != m_TextureResources.end())
    {
        it->second->MarkAsUsed();
        return it->second;
    }

    return nullptr;
}

/// <summary>
/// Unload a specific texture
/// </summary>
void ResourceManager::UnloadTexture(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);

    auto it = m_TextureResources.find(id);
    if (it != m_TextureResources.end())
    {
        std::cout << "Unloaded texture: " << id << std::endl;
        m_TextureResources.erase(it);
    }
}

/// <summary>
/// Unload textures that haven't been used for a while
/// </summary>
void ResourceManager::UnloadUnusedTextures(int maxAgeInSeconds)
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);

    auto currentTime = std::chrono::steady_clock::now();
    std::vector<std::string> texturesToRemove;

    for (const auto& pair : m_TextureResources)
    {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - pair.second->GetLastUsedTime()).count();

        if (age > maxAgeInSeconds)
        {
            texturesToRemove.push_back(pair.first);
        }
    }

    for (const auto& id : texturesToRemove)
    {
        m_TextureResources.erase(id);
    }

    if (!texturesToRemove.empty())
    {
        std::cout << "Unloaded " << texturesToRemove.size() << " unused textures, "
            << m_TextureResources.size() << " remaining" << std::endl;
    }
}

/// <summary>
/// Load a sound resource
/// </summary>
std::shared_ptr<SoundResource> ResourceManager::LoadSound(const std::string& id, const std::string& filePath,
    bool isStreaming, bool loop)
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(m_SoundMutex);

    // Check if already loaded
    auto it = m_SoundResources.find(id);
    if (it != m_SoundResources.end())
    {
        // Update usage timestamp
        m_SoundUsageTimestamps[id] = std::chrono::steady_clock::now();

        // Update looping flag if the sound is already loaded
        it->second->SetLooping(loop);
        return it->second;
    }

    // Create sound resource with proper type and looping flag
    SoundResource::ResourceType resourceType = isStreaming ?
        SoundResource::STREAMING : SoundResource::SOUND_EFFECT;

    // Create the sound resource - use PathUtils to resolve the path
    auto soundResource = std::make_shared<SoundResource>();
    std::wstring absolutePath = PathUtils::GetAbsolutePath(PathUtils::StringToWString(filePath));

    // Load the sound
    HRESULT hr = soundResource->Load(absolutePath, resourceType, loop);
    if (FAILED(hr))
    {
        std::cerr << "Failed to load sound: " << id << " from path: " << filePath << std::endl;
        return nullptr;
    }

    // Store the sound resource and usage timestamp
    m_SoundResources[id] = soundResource;
    m_SoundUsageTimestamps[id] = std::chrono::steady_clock::now();

    std::cout << "Loaded sound: " << id << " (Streaming: " << (isStreaming ? "Yes" : "No")
        << ", Loop: " << (loop ? "Yes" : "No") << ")" << std::endl;

    // Check if we need to clean up cache
    CleanupSoundCache();

    return soundResource;
}

/// <summary>
/// Get a sound resource by ID
/// </summary>
std::shared_ptr<SoundResource> ResourceManager::GetSound(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_SoundMutex);

    auto it = m_SoundResources.find(id);
    if (it != m_SoundResources.end())
    {
        // Update usage timestamp
        m_SoundUsageTimestamps[id] = std::chrono::steady_clock::now();
        return it->second;
    }

    return nullptr;
}

/// <summary>
/// Unload a specific sound
/// </summary>
void ResourceManager::UnloadSound(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_SoundMutex);

    auto it = m_SoundResources.find(id);
    if (it != m_SoundResources.end())
    {
        std::cout << "Unloaded sound: " << id << std::endl;
        m_SoundResources.erase(it);
        m_SoundUsageTimestamps.erase(id);
    }
}

/// <summary>
/// Unload sounds that haven't been used for a while
/// </summary>
void ResourceManager::UnloadUnusedSounds(int maxAgeInSeconds)
{
    std::lock_guard<std::mutex> lock(m_SoundMutex);

    auto currentTime = std::chrono::steady_clock::now();
    std::vector<std::string> soundsToRemove;

    for (const auto& pair : m_SoundUsageTimestamps)
    {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - pair.second).count();

        if (age > maxAgeInSeconds)
        {
            soundsToRemove.push_back(pair.first);
        }
    }

    for (const auto& id : soundsToRemove)
    {
        m_SoundResources.erase(id);
        m_SoundUsageTimestamps.erase(id);
    }

    if (!soundsToRemove.empty())
    {
        std::cout << "Unloaded " << soundsToRemove.size() << " unused sounds, "
            << m_SoundResources.size() << " remaining" << std::endl;
    }
}

/// <summary>
/// Unload all resources
/// </summary>
void ResourceManager::UnloadAllResources()
{
    {
        std::lock_guard<std::mutex> lockTexture(m_TextureMutex);
        size_t textureCount = m_TextureResources.size();
        m_TextureResources.clear();

        if (textureCount > 0)
        {
            std::cout << "Unloaded all " << textureCount << " textures" << std::endl;
        }
    }

    {
        std::lock_guard<std::mutex> lockSound(m_SoundMutex);
        size_t soundCount = m_SoundResources.size();
        m_SoundResources.clear();
        m_SoundUsageTimestamps.clear();

        if (soundCount > 0)
        {
            std::cout << "Unloaded all " << soundCount << " sounds" << std::endl;
        }
    }
}

/// <summary>
/// Configure the texture cache
/// </summary>
void ResourceManager::ConfigureTextureCache(size_t maxTextures, size_t minAgeSeconds)
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);

    m_MaxCachedTextures = maxTextures;
    m_MinTextureAgeSeconds = minAgeSeconds;

    std::cout << "Texture cache configured: max=" << maxTextures
        << ", min age=" << minAgeSeconds << "s" << std::endl;

    // Run cleanup with new settings if needed
    CleanupTextureCache();
}

/// <summary>
/// Configure the sound cache
/// </summary>
void ResourceManager::ConfigureSoundCache(size_t maxSounds, size_t minAgeSeconds)
{
    std::lock_guard<std::mutex> lock(m_SoundMutex);

    m_MaxCachedSounds = maxSounds;
    m_MinSoundAgeSeconds = minAgeSeconds;

    std::cout << "Sound cache configured: max=" << maxSounds
        << ", min age=" << minAgeSeconds << "s" << std::endl;

    // Run cleanup with new settings if needed
    CleanupSoundCache();
}

/// <summary>
/// Clean up the texture cache if it exceeds limits
/// </summary>
void ResourceManager::CleanupTextureCache()
{
    // Check if we're over the limit
    if (m_TextureResources.size() <= m_MaxCachedTextures)
    {
        return;
    }

    // Determine how many resources to remove
    size_t numToRemove = m_TextureResources.size() - m_MaxCachedTextures;

    // Get current time
    auto currentTime = std::chrono::steady_clock::now();

    // Prepare vector of resources for sorting
    std::vector<std::pair<std::string, std::shared_ptr<TextureResource>>> textures;
    textures.reserve(m_TextureResources.size());

    for (const auto& pair : m_TextureResources)
    {
        textures.push_back(pair);
    }

    // Sort by last used time (oldest first) - LRU policy
    std::sort(textures.begin(), textures.end(),
        [](const auto& a, const auto& b)
        {
            return a.second->GetLastUsedTime() < b.second->GetLastUsedTime();
        });

    // Remove oldest resources
    size_t removed = 0;
    std::vector<std::string> texturesToRemove;

    for (const auto& texture : textures)
    {
        // Skip if we've removed enough
        if (removed >= numToRemove)
        {
            break;
        }

        // Skip resources used recently
        auto resourceAge = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - texture.second->GetLastUsedTime()).count();

        if (resourceAge < m_MinTextureAgeSeconds)
        {
            continue;
        }

        // Add to our list of resources to remove
        texturesToRemove.push_back(texture.first);
        removed++;
    }

    // Remove the resources
    for (const auto& id : texturesToRemove)
    {
        m_TextureResources.erase(id);
    }

    if (removed > 0)
    {
        std::cout << "Texture cache cleanup: removed " << removed << " textures, "
            << m_TextureResources.size() << " remaining" << std::endl;
    }
}

/// <summary>
/// Clean up the sound cache if it exceeds limits
/// </summary>
void ResourceManager::CleanupSoundCache()
{
    // Check if we're over the limit
    if (m_SoundResources.size() <= m_MaxCachedSounds)
    {
        return;
    }

    // Determine how many resources to remove
    size_t numToRemove = m_SoundResources.size() - m_MaxCachedSounds;

    // Get current time
    auto currentTime = std::chrono::steady_clock::now();

    // Prepare vector of resources for sorting
    std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> sounds;
    sounds.reserve(m_SoundUsageTimestamps.size());

    for (const auto& pair : m_SoundUsageTimestamps)
    {
        sounds.push_back(pair);
    }

    // Sort by last used time (oldest first) - LRU policy
    std::sort(sounds.begin(), sounds.end(),
        [](const auto& a, const auto& b) 
        {
            return a.second < b.second;
        });

    // Remove oldest resources
    size_t removed = 0;
    std::vector<std::string> soundsToRemove;

    for (const auto& sound : sounds)
    {
        // Skip if we've removed enough
        if (removed >= numToRemove)
        {
            break;
        }

        // Skip resources used recently
        auto resourceAge = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - sound.second).count();

        if (resourceAge < m_MinSoundAgeSeconds)
        {
            continue;
        }

        // Add to our list of resources to remove
        soundsToRemove.push_back(sound.first);
        removed++;
    }

    // Remove the resources
    for (const auto& id : soundsToRemove)
    {
        m_SoundResources.erase(id);
        m_SoundUsageTimestamps.erase(id);
    }

    if (removed > 0)
    {
        std::cout << "Sound cache cleanup: removed " << removed << " sounds, "
            << m_SoundResources.size() << " remaining" << std::endl;
    }
}