// ResourceManager.cpp
#include "ResourceManager.h"
#include "Renderer.h"
#include "SoundResource.h"
#include <algorithm>
#include <iostream>

bool ResourceManager::Initialize(Renderer* renderer)
{
    m_Renderer = renderer;
    std::cout << "ResourceManager initialized" << std::endl;
    return true;
}

void ResourceManager::Shutdown()
{
    UnloadAllResources();
    std::cout << "ResourceManager shut down" << std::endl;
}

std::shared_ptr<TextureResource> ResourceManager::LoadTexture(const std::string& id, const std::string& filePath)
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    // Check if already loaded
    auto it = m_Resources.find(id);
    if (it != m_Resources.end())
    {
        auto texture = std::dynamic_pointer_cast<TextureResource>(it->second);
        if (texture)
        {
            texture->MarkAsUsed();
            return texture;
        }
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
    m_Resources[id] = textureResource;

    std::cout << "Loaded texture: " << id << " (" << width << "x" << height << ")" << std::endl;

    // Check if we need to clean up cache
    CleanupResourceCache();

    return textureResource;
}

std::shared_ptr<TextureResource> ResourceManager::GetTexture(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    auto it = m_Resources.find(id);
    if (it != m_Resources.end())
    {
        auto textureResource = std::dynamic_pointer_cast<TextureResource>(it->second);
        if (textureResource)
        {
            textureResource->MarkAsUsed();
            return textureResource;
        }
    }

    return nullptr;
}

std::shared_ptr<SoundResourceWrapper> ResourceManager::LoadSound(const std::string& id, const std::string& filePath, bool isStreaming)
{
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    // Check if already loaded
    auto it = m_Resources.find(id);
    if (it != m_Resources.end())
    {
        auto sound = std::dynamic_pointer_cast<SoundResourceWrapper>(it->second);
        if (sound)
        {
            sound->MarkAsUsed();
            return sound;
        }
    }

    // Get absolute path using PathUtils
    std::wstring absolutePath = PathUtils::GetAbsolutePath(PathUtils::StringToWString(filePath));

    // Create resource type based on streaming flag
    SoundResource::ResourceType resourceType = isStreaming ?
        SoundResource::STREAMING : SoundResource::SOUND_EFFECT;

    // Create sound resource
    auto soundResource = std::make_shared<SoundResource>(absolutePath, resourceType);
    auto soundWrapper = std::make_shared<SoundResourceWrapper>(id, soundResource);

    m_Resources[id] = soundWrapper;

    std::cout << "Loaded sound: " << id << " (Streaming: " << (isStreaming ? "Yes" : "No") << ")" << std::endl;

    // Check if we need to clean up cache
    CleanupResourceCache();

    return soundWrapper;
}

std::shared_ptr<SoundResourceWrapper> ResourceManager::GetSound(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    auto it = m_Resources.find(id);
    if (it != m_Resources.end())
    {
        auto soundResource = std::dynamic_pointer_cast<SoundResourceWrapper>(it->second);
        if (soundResource)
        {
            soundResource->MarkAsUsed();
            return soundResource;
        }
    }

    return nullptr;
}

void ResourceManager::UnloadResource(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    auto it = m_Resources.find(id);
    if (it != m_Resources.end())
    {
        std::cout << "Unloaded resource: " << id << std::endl;
        m_Resources.erase(it);
    }
}

void ResourceManager::UnloadUnusedResources(int maxAgeInSeconds)
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    auto currentTime = std::chrono::steady_clock::now();
    std::vector<std::string> resourcesToRemove;

    for (const auto& pair : m_Resources)
    {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - pair.second->GetLastUsedTime()).count();

        if (age > maxAgeInSeconds)
        {
            resourcesToRemove.push_back(pair.first);
        }
    }

    for (const auto& id : resourcesToRemove)
    {
        m_Resources.erase(id);
    }

    if (!resourcesToRemove.empty())
    {
        std::cout << "Unloaded " << resourcesToRemove.size() << " unused resources, "
            << m_Resources.size() << " remaining" << std::endl;
    }
}

void ResourceManager::UnloadAllResources()
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    size_t count = m_Resources.size();
    m_Resources.clear();

    if (count > 0)
    {
        std::cout << "Unloaded all " << count << " resources" << std::endl;
    }
}

void ResourceManager::ConfigureCache(size_t maxCachedResources, size_t minResourceAgeSeconds)
{
    std::lock_guard<std::mutex> lock(m_ResourceMutex);

    m_MaxCachedResources = maxCachedResources;
    m_MinResourceAgeSeconds = minResourceAgeSeconds;

    std::cout << "Resource cache configured: max=" << maxCachedResources
        << ", min age=" << minResourceAgeSeconds << "s" << std::endl;

    // Run cleanup with new settings if needed
    CleanupResourceCache();
}

void ResourceManager::CleanupResourceCache()
{
    // Check if we're over the limit
    if (m_Resources.size() <= m_MaxCachedResources)
    {
        return;
    }

    // Determine how many resources to remove
    size_t numToRemove = m_Resources.size() - m_MaxCachedResources;

    // Get current time
    auto currentTime = std::chrono::steady_clock::now();

    // Prepare vector of resources for sorting
    std::vector<std::pair<std::string, std::shared_ptr<Resource>>> resources;
    resources.reserve(m_Resources.size());

    for (const auto& pair : m_Resources)
    {
        resources.push_back(pair);
    }

    // Sort by last used time (oldest first) - LRU policy
    std::sort(resources.begin(), resources.end(),
        [](const auto& a, const auto& b) {
            return a.second->GetLastUsedTime() < b.second->GetLastUsedTime();
        });

    // Remove oldest resources
    size_t removed = 0;
    std::vector<std::string> resourcesToRemove;

    for (const auto& resource : resources)
    {
        // Skip if we've removed enough
        if (removed >= numToRemove)
        {
            break;
        }

        // Skip resources used recently
        auto resourceAge = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - resource.second->GetLastUsedTime()).count();

        if (resourceAge < m_MinResourceAgeSeconds)
        {
            continue;
        }

        // Add to our list of resources to remove
        resourcesToRemove.push_back(resource.first);
        removed++;
    }

    // Remove the resources
    for (const auto& id : resourcesToRemove)
    {
        m_Resources.erase(id);
    }

    if (removed > 0)
    {
        std::cout << "Resource cache cleanup: removed " << removed << " resources, "
            << m_Resources.size() << " remaining" << std::endl;
    }
}