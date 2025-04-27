/// <file>
/// <summary>
/// Audio Manager using XAudio2
/// for the audio engine subsystem of the game engine
/// Provides a public interface for playing, stopping, and managing audio resources.
/// </summary>
/// 
/// References:
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-apis-portal
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-27</date>
/// </file>

#include "AudioManager.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "PathUtils.h"
#include "ResourceManager.h"

// Define global singleton accessor
AudioManager& gAudioManager = AudioManager::GetInstance();

AudioManager::AudioManager()
    : pXAudio2(nullptr),
    pMasteringVoice(nullptr),
    masterVolume(1.0f),
    channelMask(0),
    channels(0),
    maxCachedResources(64),
    minResourceAgeSeconds(10),
    maxSourceVoices(32)
{
    // do nothing
}

AudioManager::~AudioManager()
{
    // do nothing
}

/// <summary>
/// Initializes the audio system.
/// </summary>
/// <returns></returns>
bool AudioManager::startUp(size_t maxCachedResources, size_t minResourceAge)
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != S_FALSE && hr != RPC_E_CHANGED_MODE)
    {
        std::cerr << "Failed to initialize COM: " << std::hex << hr << std::dec << std::endl;
        return false;
    }

    // Create XAudio2 instance
    hr = XAudio2Create(&pXAudio2, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create XAudio2 instance: " << std::hex << hr << std::dec << std::endl;
        CoUninitialize();
        return false;
    }

    // Create mastering voice
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create mastering voice: " << std::hex << hr << std::dec << std::endl;
        pXAudio2.Reset();
        CoUninitialize();
        return false;
    }

    // Get the channel mask from mastering voice
    hr = pMasteringVoice->GetChannelMask(&channelMask);
    if (FAILED(hr) || channelMask == 0)
    {
        std::cerr << "Failed to get channel mask or invalid mask: " << std::hex << hr << std::dec << std::endl;
        // Default to stereo if we can't get a valid mask
        channelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
    }

    // Get voice details to determine channel count
    XAUDIO2_VOICE_DETAILS voiceDetails;
    pMasteringVoice->GetVoiceDetails(&voiceDetails);
    channels = voiceDetails.InputChannels;

    if (channels == 0)
    {
        std::cerr << "Invalid channel count: " << channels << std::endl;
        channels = 2;  // Default to stereo
    }

    // Set initial master volume
    pMasteringVoice->SetVolume(masterVolume);

	// Configure cache settings
	ConfigureCache(maxCachedResources, minResourceAge);

    std::cout << "AudioManager initialized successfully" << std::endl;
    return true;
}

/// <summary>
/// Shut down the audio system.
/// </summary>
void AudioManager::shutDown()
{
    // Stop all sounds first
    StopAllSounds();

    // Clear resources and source voices
    {
        std::lock_guard<std::mutex> lock(resourceMutex);

        for (auto& pair : sourceVoices)
        {
            if (pair.second)
            {
                pair.second->DestroyVoice();
                pair.second = nullptr;
            }
        }
        sourceVoices.clear();
        soundResources.clear();
        resourceUsage.clear();
    }

    // Release XAudio2 resources
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }

    pXAudio2.Reset();
    CoUninitialize();

    std::cout << "AudioManager shut down successfully" << std::endl;
}

/// <summary>
/// Plays a sound from the specified file path.
/// </summary>
/// <param name="filePath">The filepath of the audio file, relative to the root.</param>
/// <param name="isSoundEffect">TRUE:Repeated SFX, FALSE:Streaming audio</param>
/// <param name="volume">Sound volume, between 0.0 and 1.0</param>
/// <returns></returns>
HRESULT AudioManager::PlaySound(const std::string& filePath, bool isSoundEffect, float volume, bool loop)
{
    if (!pXAudio2 || !pMasteringVoice)
        return E_FAIL;

    std::wstring wFilePath = ConvertToWideString(filePath);

    // Get or load the sound resource
    std::shared_ptr<SoundResource> resource = GetOrLoadResource(wFilePath, isSoundEffect, loop);
    if (!resource)
        return E_FAIL;

    // Get the source voice if it exists, or create a new one
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        auto iter = sourceVoices.find(wFilePath);
        if (iter != sourceVoices.end())
        {
            pSourceVoice = iter->second;

            // Stop the voice before reusing it
            pSourceVoice->Stop(0);
            pSourceVoice->FlushSourceBuffers();
        }
    }

    // Play the sound with volume control
    HRESULT hr = resource->Play(pXAudio2.Get(), &pSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    // Store the source voice and update usage statistics
    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        sourceVoices[wFilePath] = pSourceVoice;
        UpdateResourceUsage(wFilePath);
    }

    return S_OK;
}

/// <summary>
/// Plays a sound using a provided SoundResource.
/// </summary>
/// <param name="pSoundResource">Pointer to the sound resource to play</param>
/// <param name="volume">Sound volume, between 0.0 and 1.0</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::PlaySound(SoundResource* pSoundResource, float volume)
{
    if (!pXAudio2 || !pMasteringVoice || !pSoundResource)
        return E_INVALIDARG;

    const std::wstring& resourcePath = pSoundResource->GetFilePath();

    // Get the source voice if it exists, or create a new one
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        auto iter = sourceVoices.find(resourcePath);
        if (iter != sourceVoices.end())
        {
            pSourceVoice = iter->second;

            // Stop the voice before reusing it
            pSourceVoice->Stop(0);
            pSourceVoice->FlushSourceBuffers();
        }
    }

    // Play the sound with volume control
    HRESULT hr = pSoundResource->Play(pXAudio2.Get(), &pSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    // Store the source voice in the AudioManager
    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        sourceVoices[resourcePath] = pSourceVoice;
    }

    return S_OK;
}

/// <summary>
/// Stops the sound associated with the specified file path.
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
HRESULT AudioManager::StopSound(const std::string& filePath)
{
    if (!pXAudio2 || !pMasteringVoice)
        return E_FAIL;

    std::wstring wFilePath = ConvertToWideString(filePath);

    std::lock_guard<std::mutex> lock(resourceMutex);

    // Find the resource
    auto resourceIter = soundResources.find(wFilePath);
    if (resourceIter == soundResources.end())
        return E_FAIL;

    // Find the source voice
    auto voiceIter = sourceVoices.find(wFilePath);
    if (voiceIter == sourceVoices.end())
        return E_FAIL;

    // Stop the sound
    return resourceIter->second->Stop(voiceIter->second);
}

/// <summary>
/// Stops all currently playing sounds.
/// </summary>
void AudioManager::StopAllSounds()
{
    if (!pXAudio2 || !pMasteringVoice)
        return;

    std::lock_guard<std::mutex> lock(resourceMutex);

    // Stop each sound
    for (auto& pair : soundResources)
    {
        auto voiceIter = sourceVoices.find(pair.first);
        if (voiceIter != sourceVoices.end() && voiceIter->second)
        {
            pair.second->Stop(voiceIter->second);
        }
    }
}

/// <summary>
/// Sets the volume for a specific sound.
/// </summary>
/// <param name="filePath">The filepath of the audio file, relative to the project root.</param>
/// <param name="volume">The volume of the sound, between 0.0 and 1.0</param>
/// <returns></returns>
HRESULT AudioManager::SetSoundVolume(const std::string& filePath, float volume)
{
    if (!pXAudio2 || !pMasteringVoice)
        return E_FAIL;

    std::wstring wFilePath = ConvertToWideString(filePath);

    std::lock_guard<std::mutex> lock(resourceMutex);

    // Find the source voice
    auto voiceIter = sourceVoices.find(wFilePath);
    if (voiceIter == sourceVoices.end() || !voiceIter->second)
        return E_FAIL;

    // Find the resource
    auto resourceIter = soundResources.find(wFilePath);
    if (resourceIter == soundResources.end())
        return E_FAIL;

    // Set volume
    return resourceIter->second->SetVolume(voiceIter->second, volume);
}

/// <summary>
/// Sets the master volume for all sounds.
/// </summary>
/// <param name="volume"></param>
/// <returns></returns>
HRESULT AudioManager::SetMasterVolume(float volume)
{
    if (!pMasteringVoice)
        return E_FAIL;

    // Clamp volume between 0.0 and 1.0
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    else if (volume > 1.0f)
    {
        volume = 1.0f;
    }

    // Store the master volume
    masterVolume = volume;

    return pMasteringVoice->SetVolume(volume);
}

/// <summary>
/// Loads a sound resource from the specified file path.
/// </summary>
/// <param name="filePath"></param>
/// <param name="isSoundEffect"></param>
/// <returns></returns>
std::shared_ptr<SoundResource> AudioManager::GetOrLoadResource(const std::wstring& filePath, bool isSoundEffect, bool loop)
{
    std::lock_guard<std::mutex> lock(resourceMutex);

    // Check if resource already exists
    auto iter = soundResources.find(filePath);
    if (iter != soundResources.end())
    {
        // Update usage statistics
        UpdateResourceUsage(filePath);

        // Update looping status if it's changed
        iter->second->SetLooping(loop);

        return iter->second;
    }

    // Create new resource
    auto resource = std::make_shared<SoundResource>();

    // Select resource type based on sound type
    SoundResource::ResourceType resourceType = isSoundEffect ?
        SoundResource::SOUND_EFFECT : SoundResource::STREAMING;

    // Load the resource with looping flag
    HRESULT hr = resource->Load(filePath, resourceType, loop);
    if (FAILED(hr))
    {
        std::wcerr << L"Failed to load sound resource: " << filePath << std::endl;
        return nullptr;
    }

    // Store and return the resource without checking for cleanup
    soundResources[filePath] = resource;

    // Initialize usage tracking for this resource
    resourceUsage[filePath] = ResourceUsageInfo(filePath);

    return resource;
}

/// <summary>
/// Update the usage statistics for a resource
/// </summary>
/// <param name="filePath">Path to the resource</param>
void AudioManager::UpdateResourceUsage(const std::wstring& filePath)
{
    auto iter = resourceUsage.find(filePath);
    if (iter != resourceUsage.end())
    {
        // Update existing entry
        iter->second.Used();
    }
    else
    {
        // Create new entry
        resourceUsage[filePath] = ResourceUsageInfo(filePath);
    }
}

/// <summary>
/// Clean up resource cache to maintain memory limits
/// This function should be periodically called during the game engine loop to clean up resources
/// </summary>
/// <param name="maxResourcesOverride">If specified, clean up to this limit instead of the configured maximum</param>
void AudioManager::CleanupResourceCache(size_t maxResourcesOverride)
{
    // Create a list of resources to remove, outside of the lock
    std::vector<std::wstring> resourcesToRemove;
    bool shouldCleanupVoices = false;

    // Get resources to remove
    {
        std::lock_guard<std::mutex> lock(resourceMutex);

        // Use override if provided, otherwise use the configured maximum
        size_t targetMax = (maxResourcesOverride > 0) ? maxResourcesOverride : maxCachedResources;

        // If we're under the limit, no cleanup needed
        if (soundResources.size() <= targetMax)
        {
            // Check if we need to clean up voices
            shouldCleanupVoices = (sourceVoices.size() > maxSourceVoices);

            // If we don't need to clean up voices either, return early
            if (!shouldCleanupVoices)
                return;
        }

        // Only process resource cleanup if we're over the limit
        if (soundResources.size() > targetMax)
        {
            // Determine how many resources to remove
            size_t numToRemove = soundResources.size() - targetMax;
            if (numToRemove > 0)
            {
                // Get current time
                auto currentTime = std::chrono::steady_clock::now();

                // Prepare vector of resources for sorting
                std::vector<std::pair<std::wstring, ResourceUsageInfo>> resources;
                resources.reserve(resourceUsage.size());  // Pre-allocate for efficiency

                for (const auto& pair : resourceUsage)
                {
                    resources.push_back(pair);
                }

                // Sort by last used time (oldest first) - LRU policy
                std::sort(resources.begin(), resources.end(),
                    [](const auto& a, const auto& b) {
                        return a.second.lastUsedTime < b.second.lastUsedTime;
                    });

                // Count resources removed
                size_t removed = 0;

                // Iterate through sorted resources to identify what to remove
                for (const auto& resource : resources)
                {
                    // Skip if we've removed enough
                    if (removed >= numToRemove)
                        break;

                    const std::wstring& path = resource.first;

                    // Skip resources used recently (within minResourceAgeSeconds)
                    auto resourceAge = std::chrono::duration_cast<std::chrono::seconds>(
                        currentTime - resource.second.lastUsedTime).count();

                    if (resourceAge < minResourceAgeSeconds)
                        continue;

                    // Add to our list of resources to remove
                    resourcesToRemove.push_back(path);
                    removed++;
                }

                std::cout << "Audio cache cleanup: identified " << removed << " resources to remove" << std::endl;
            }
        }

        // Set flag to clean up source voices after
        shouldCleanupVoices = true;
    }

    // Now actually remove the resources
    if (!resourcesToRemove.empty())
    {
        size_t removedCount = 0;

        // Process each resource to remove
        for (const auto& path : resourcesToRemove)
        {
            // Handle any stop operations outside the lock if possible

            // Now get the lock to modify collections
            std::lock_guard<std::mutex> lock(resourceMutex);

            // Check if this resource has a source voice
            auto voiceIter = sourceVoices.find(path);
            if (voiceIter != sourceVoices.end())
            {
                // Stop and destroy the voice
                if (voiceIter->second)
                {
                    auto resourceIter = soundResources.find(path);
                    if (resourceIter != soundResources.end())
                    {
                        resourceIter->second->Stop(voiceIter->second);
                    }

                    voiceIter->second->DestroyVoice();
                    voiceIter->second = nullptr;
                }
                sourceVoices.erase(voiceIter);
            }

            // Remove the resource
            soundResources.erase(path);
            resourceUsage.erase(path);
            removedCount++;

            std::wcout << L"Removed unused audio resource: " << path << std::endl;
        }

        std::cout << "Audio cache cleanup: removed " << removedCount << " resources, "
            << soundResources.size() << " remaining" << std::endl;
    }

    // Clean up source voices if needed, but do it outside of any locks to prevent deadlock
    if (shouldCleanupVoices)
    {
        CleanupSourceVoicePool();
    }
}

/// <summary>
/// Clean up source voice pool to maintain limits
/// This function should be called with the resourceMutex already locked
/// </summary>
void AudioManager::CleanupSourceVoicePool()
{
    // If we're under the limit, no cleanup needed
    if (sourceVoices.size() <= maxSourceVoices)
    {
        return;
    }

    std::cout << "Source voice pool cleanup: " << sourceVoices.size()
        << " voices, limit " << maxSourceVoices << std::endl;

    // Determine how many voices to remove
    size_t numToRemove = sourceVoices.size() - maxSourceVoices;
    if (numToRemove <= 0)
        return;

    // Prepare vector of source voices for sorting by LRU
    std::vector<std::pair<std::wstring, std::chrono::steady_clock::time_point>> voices;
    voices.reserve(sourceVoices.size());

    // Get last used time for each voice from resourceUsage
    for (const auto& pair : sourceVoices)
    {
        const std::wstring& path = pair.first;
        auto usageIter = resourceUsage.find(path);

        if (usageIter != resourceUsage.end())
        {
            voices.push_back(std::make_pair(path, usageIter->second.lastUsedTime));
        }
        else
        {
            // If no usage info, treat as very old (epoch time)
            voices.push_back(std::make_pair(path, std::chrono::steady_clock::time_point()));
        }
    }

    // Sort by last used time (oldest first) - LRU policy
    std::sort(voices.begin(), voices.end(),
        [](const auto& a, const auto& b) 
        {
            return a.second < b.second;
        });

    // Remove oldest voices up to the limit
    size_t removed = 0;
    for (size_t i = 0; i < numToRemove && i < voices.size(); i++)
    {
        const std::wstring& path = voices[i].first;
        auto voiceIter = sourceVoices.find(path);

        if (voiceIter != sourceVoices.end() && voiceIter->second)
        {
            // Stop the voice if it's playing
            auto resourceIter = soundResources.find(path);
            if (resourceIter != soundResources.end())
            {
                resourceIter->second->Stop(voiceIter->second);
            }

            // Destroy the voice
            voiceIter->second->DestroyVoice();
            voiceIter->second = nullptr;
            sourceVoices.erase(voiceIter);
            removed++;

            std::wcout << L"Removed unused source voice: " << path << std::endl;
        }
    }

    std::cout << "Source voice pool cleanup: removed " << removed
        << " voices, " << sourceVoices.size() << " remaining" << std::endl;
}

/// <summary>
/// Configure cache settings
/// </summary>
/// <param name="maxCachedResources">Maximum number of resources to keep in cache</param>
/// <param name="minResourceAge">Minimum age in seconds before a resource can be removed</param>
/// <param name="maxSourceVoices">Maximum number of source voices to keep</param>
void AudioManager::ConfigureCache(size_t maxCachedResources, size_t minResourceAge, size_t maxSourceVoices)
{
    std::lock_guard<std::mutex> lock(resourceMutex);

    this->maxCachedResources = maxCachedResources;
    this->minResourceAgeSeconds = minResourceAge;
    this->maxSourceVoices = maxSourceVoices;

    std::cout << "Audio cache configured: max resources=" << maxCachedResources
        << ", min age=" << minResourceAge << "s, "
        << "max source voices=" << maxSourceVoices
        << ", using LRU policy" << std::endl;

    // Run cleanup with new settings if needed
    if (soundResources.size() > maxCachedResources || sourceVoices.size() > maxSourceVoices)
    {
        CleanupResourceCache();
    }
}

/// <summary>
/// Helper function to convert a narrow string to a wide string.
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring AudioManager::ConvertToWideString(const std::string& str)
{
    if (str.empty()) return std::wstring();

    // Calculate required buffer size
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    // Create wide string of required size
    std::wstring wstr(size_needed, 0);

    // Perform the conversion
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);

    return wstr;
}

// For use by SoundResource.cpp
std::wstring GetProjectRoot()
{
    return PathUtils::GetProjectRoot();
}

/// <summary>
/// Set the environment type for the audio.
/// </summary>
/// <param name="envType"></param>
/// <returns></returns>
HRESULT AudioManager::SetEnvironment(EnvironmentType envType)
{
    if (!pXAudio2 || !pMasteringVoice)
        return E_FAIL;

    // Always start by clearing any existing effect chain
    HRESULT hr = pMasteringVoice->SetEffectChain(nullptr);
    if (FAILED(hr))
    {
        std::cerr << "Failed to clear effect chain: " << std::hex << hr << std::dec << std::endl;
        return hr;
    }

    // For ENV_NORMAL, we just leave the effect chain empty
    if (envType == ENV_NORMAL)
    {
        std::cout << "Environment set to NORMAL (no effects)" << std::endl;
        return S_OK;
    }

    // For other environment types, create and apply the appropriate reverb effect
    IUnknown* pXAPO = nullptr;
    XAUDIO2_EFFECT_DESCRIPTOR effects[1];
    XAUDIO2_EFFECT_CHAIN effectChain;

    // Create reverb effect
    hr = XAudio2CreateReverb(&pXAPO);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create reverb effect: " << std::hex << hr << std::dec << std::endl;
        return hr;
    }

    effects[0].pEffect = pXAPO;
    effects[0].InitialState = true;
    effects[0].OutputChannels = channels;

    effectChain.EffectCount = 1;
    effectChain.pEffectDescriptors = effects;

    // Set effect chain
    hr = pMasteringVoice->SetEffectChain(&effectChain);
    if (FAILED(hr))
    {
        std::cerr << "Failed to set effect chain: " << std::hex << hr << std::dec << std::endl;
        pXAPO->Release();
        return hr;
    }

    // Configure reverb parameters based on environment
    XAUDIO2FX_REVERB_PARAMETERS reverbParameters{};

    // First, initialize with default values
    reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
    reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
    reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
    reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
    reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
    reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
    reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
    reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
    reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
    reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
    reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
    reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
    reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
    reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
    reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
    reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
    reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
    reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
    reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
    reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

    // Now override specific parameters for each environment type
    switch (envType)
    {
    case ENV_CAVE:
        // Cave-like echo
        reverbParameters.ReverbDelay = 85;
        reverbParameters.DecayTime = 3.0f;
        reverbParameters.Density = 100.0f;
        reverbParameters.RoomFilterFreq = 5000.0f;
        reverbParameters.RoomFilterMain = 0.75f;
        reverbParameters.RoomFilterHF = 0.5f;
        reverbParameters.WetDryMix = 100.0f;  
        reverbParameters.ReflectionsGain = 1.3f;
        reverbParameters.ReverbGain = 1.5f;
        reverbParameters.EarlyDiffusion = 10;
        reverbParameters.LateDiffusion = 10;
        reverbParameters.RoomSize = 100.0f;  
        std::cout << "Environment set to CAVE" << std::endl;
        break;

    case ENV_UNDERWATER:
        // Underwater muffled sound
        reverbParameters.DecayTime = 2.5f;                 
        reverbParameters.Density = 100.0f;                
        reverbParameters.RoomFilterFreq = 1000.0f;        
        reverbParameters.RoomFilterMain = 0.9f;            
        reverbParameters.RoomFilterHF = 0.05f;             
        reverbParameters.WetDryMix = 70.0f;               
        reverbParameters.HighEQGain = 0.15f;               
        reverbParameters.LowEQGain = 2.0f;                
        reverbParameters.RoomSize = 40.0f;                
        reverbParameters.ReflectionsGain = 0.3f;       
        reverbParameters.ReverbGain = 1.2f;               
        reverbParameters.EarlyDiffusion = 5;               
        reverbParameters.LateDiffusion = 5;              
        std::cout << "Environment set to UNDERWATER" << std::endl;
        break;

    case ENV_LARGE_HALL:
        // Big spaces like boss rooms
        reverbParameters.ReverbDelay = 100;
        reverbParameters.DecayTime = 4.0f;
        reverbParameters.Density = 100.0f;
        reverbParameters.RoomFilterFreq = 8000.0f;
        reverbParameters.RoomFilterMain = 0.6f;
        reverbParameters.RoomFilterHF = 0.8f;
        reverbParameters.WetDryMix = 80.0f;    
        reverbParameters.ReflectionsGain = 1.2f;
        reverbParameters.ReverbGain = 1.4f;
        reverbParameters.RoomSize = 100.0f;    
        std::cout << "Environment set to LARGE_HALL" << std::endl;
        break;

    default:
        // We shouldn't reach this case since ENV_NORMAL is handled above
        // But just in case, set very minimal reverb
        reverbParameters.WetDryMix = 0.0f;  // No wet signal = no reverb
        std::cout << "Environment set to default" << std::endl;
        break;
    }

    // Set reverb parameters
    hr = pMasteringVoice->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters));
    if (FAILED(hr))
    {
        std::cerr << "Failed to set reverb parameters: " << std::hex << hr << std::dec << std::endl;
    }

    // Release XAPO interface
    pXAPO->Release();

    return hr;
}