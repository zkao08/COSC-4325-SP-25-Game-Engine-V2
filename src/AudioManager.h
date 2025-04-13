// AudioManager.h
#pragma once
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <wrl/client.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <queue>
#include <chrono>
#include "SoundResource.h"

// Room/Environment types for sound effects
enum EnvironmentType
{
    ENV_NORMAL,
    ENV_CAVE,
    ENV_UNDERWATER,
    ENV_LARGE_HALL
};

// Resource usage tracking structure
struct ResourceUsageInfo
{
    std::wstring path;
    std::chrono::steady_clock::time_point lastUsedTime;

    ResourceUsageInfo() {}

    ResourceUsageInfo(const std::wstring& p)
        : path(p),
        lastUsedTime(std::chrono::steady_clock::now()) { }

    // Update usage timestamp
    void Used()
    {
        lastUsedTime = std::chrono::steady_clock::now();
    }
};

// AudioManager singleton
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    // Singleton access
    static AudioManager& GetInstance()
    {
        static AudioManager instance;
        return instance;
    }

    // Initialize the audio system
    bool startUp(size_t maxCachedResources = 64,
                 size_t minResourceAge = 10);

    // Shutdown the audio system
    void shutDown();

    // Play a sound with given volume (0.0f to 1.0f)
    HRESULT PlaySound(const std::string& filePath, bool isSoundEffect = true, float volume = 1.0f);

    // Stop a specific sound
    HRESULT StopSound(const std::string& filePath);

    // Stop all sounds
    void StopAllSounds();

    // Set volume for a specific sound (0.0f to 1.0f)
    HRESULT SetSoundVolume(const std::string& filePath, float volume);

    // Set master volume (0.0f to 1.0f)
    HRESULT SetMasterVolume(float volume);

    // Get master volume
    float GetMasterVolume() const { return masterVolume; }

    // Room/Environment Effects
    HRESULT SetEnvironment(EnvironmentType envType);

    // Memory management - cleanup cache
    void CleanupResourceCache(size_t maxResourcesOverride = 0);

    // Get project root directory
    std::wstring GetProjectRoot();

    // Configure cache settings
    void ConfigureCache(size_t maxCachedResources = 64,
        size_t minResourceAge = 10);

private:
    // XAudio2 interfaces
    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;

    // Sound resources and source voices
    std::unordered_map<std::wstring, std::shared_ptr<SoundResource>> soundResources;
    std::unordered_map<std::wstring, IXAudio2SourceVoice*> sourceVoices;

    // Resource usage tracking
    std::unordered_map<std::wstring, ResourceUsageInfo> resourceUsage;

    // Cache settings
    size_t maxCachedResources;
    size_t minResourceAgeSeconds;

    // Mutex for thread safety
    std::mutex resourceMutex;

    // Master volume level
    float masterVolume;

    // Helper methods
    std::shared_ptr<SoundResource> GetOrLoadResource(const std::wstring& filePath, bool isSoundEffect);
    void UpdateResourceUsage(const std::wstring& filePath);

    // Convert path string from narrow to wide
    std::wstring ConvertToWideString(const std::string& str);

    // Cached speaker configuration
    DWORD channelMask;
    UINT32 channels;
};

// Global singleton accessor
extern AudioManager& gAudioManager;