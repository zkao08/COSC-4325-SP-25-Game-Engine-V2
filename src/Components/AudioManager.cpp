/// <file>
/// <summary>
/// Audio Manager using XAudio2
/// for the audio engine subsystem of the game engine
/// Provides a public interface for playing, stopping, and managing audio playback.
/// Delegates resource management to ResourceManager.
/// </summary>
/// 
/// References:
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-apis-portal
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-30</date>
/// </file>

#include "AudioManager.h"
#include "ResourceManager.h"
#include "SoundResource.h"
#include "PathUtils.h"
#include <iostream>
#include <algorithm>

/// <summary>
/// Get the singleton instance of AudioManager
/// </summary>
AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

/// <summary>
/// Initialize the AudioManager with default values
/// </summary>
AudioManager::AudioManager()
    : m_MasteringVoice(nullptr),
    m_MasterVolume(1.0f),
    m_ChannelMask(0),
    m_Channels(0),
    m_MaxSourceVoices(64)
{
    // Empty constructor
}

/// <summary>
/// Clean up AudioManager resources
/// </summary>
AudioManager::~AudioManager()
{
    // Empty destructor - cleanup in shutDown()
}

/// <summary>
/// Initialize the audio system
/// </summary>
/// <returns>TRUE if initialization succeeded, FALSE otherwise</returns>
bool AudioManager::startUp()
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != S_FALSE && hr != RPC_E_CHANGED_MODE)
    {
        std::cerr << "Failed to initialize COM: " << std::hex << hr << std::dec << std::endl;
        return false;
    }

    // Create XAudio2 instance
    hr = XAudio2Create(&m_XAudio2, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create XAudio2 instance: " << std::hex << hr << std::dec << std::endl;
        CoUninitialize();
        return false;
    }

    // Create mastering voice
    hr = m_XAudio2->CreateMasteringVoice(&m_MasteringVoice);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create mastering voice: " << std::hex << hr << std::dec << std::endl;
        m_XAudio2.Reset();
        CoUninitialize();
        return false;
    }

    // Get the channel mask from mastering voice
    hr = m_MasteringVoice->GetChannelMask(&m_ChannelMask);
    if (FAILED(hr) || m_ChannelMask == 0)
    {
        std::cerr << "Failed to get channel mask or invalid mask: " << std::hex << hr << std::dec << std::endl;
        // Default to stereo if we can't get a valid mask
        m_ChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
    }

    // Get voice details to determine channel count
    XAUDIO2_VOICE_DETAILS voiceDetails;
    m_MasteringVoice->GetVoiceDetails(&voiceDetails);
    m_Channels = voiceDetails.InputChannels;

    if (m_Channels == 0)
    {
        std::cerr << "Invalid channel count: " << m_Channels << std::endl;
        m_Channels = 2;  // Default to stereo
    }

    // Set initial master volume
    m_MasteringVoice->SetVolume(m_MasterVolume);
    return true;
}

/// <summary>
/// Shutdown the audio system and clean up resources
/// </summary>
void AudioManager::shutDown()
{
    // Stop all sounds first
    StopAllSounds();

    // Clear source voices
    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);

        for (auto& pair : m_SourceVoices)
        {
            if (pair.second)
            {
                pair.second->DestroyVoice();
                pair.second = nullptr;
            }
        }
        m_SourceVoices.clear();
        m_VoiceLastUsedTime.clear();
    }

    // Release XAudio2 resources
    if (m_MasteringVoice)
    {
        m_MasteringVoice->DestroyVoice();
        m_MasteringVoice = nullptr;
    }

    m_XAudio2.Reset();
    CoUninitialize();

    std::cout << "AudioManager shut down successfully" << std::endl;
}

/// <summary>
/// Play a sound using its resource ID from the ResourceManager
/// </summary>
/// <param name="soundId">ID of the sound in the ResourceManager</param>
/// <param name="volume">Volume level between 0.0 and 1.0</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::PlaySound(const std::string& soundId, float volume)
{
    if (!m_XAudio2 || !m_MasteringVoice)
        return E_FAIL;

    // Get the sound resource from ResourceManager
    auto soundResource = ResourceManager::GetInstance().GetSound(soundId);
    if (!soundResource)
        return E_FAIL;

    // Try to get an existing source voice
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);
        auto it = m_SourceVoices.find(soundId);
        if (it != m_SourceVoices.end() && it->second != nullptr)
        {
            pSourceVoice = it->second;

            // Check if the voice is valid and reset it
            XAUDIO2_VOICE_STATE state;
            try 
            {
                pSourceVoice->GetState(&state);
                pSourceVoice->Stop(0);
                pSourceVoice->FlushSourceBuffers();
            }
            catch (...) 
            {
                // If the voice is invalid, create a new one
                pSourceVoice = nullptr;
            }
        }
    }

    // Play the sound
    HRESULT hr = soundResource->Play(m_XAudio2.Get(), &pSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    // Update the source voice pool
    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);

        // If we're over the limit, try to clean up non-playing voices
        if (m_SourceVoices.size() >= m_MaxSourceVoices &&
            m_SourceVoices.find(soundId) == m_SourceVoices.end())
        {
            CleanupSourceVoicePool(false);
        }

        // Store the source voice and update last used time
        m_SourceVoices[soundId] = pSourceVoice;
        m_VoiceLastUsedTime[soundId] = std::chrono::steady_clock::now();
    }

    return S_OK;
}

/// <summary>
/// Play a sound directly using a SoundResource, bypassing ResourceManager
/// </summary>
/// <param name="pSoundResource">Pointer to the sound resource</param>
/// <param name="volume">Volume level between 0.0 and 1.0</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::PlaySoundDirect(SoundResource* pSoundResource, float volume)
{
    if (!m_XAudio2 || !m_MasteringVoice || !pSoundResource)
        return E_INVALIDARG;

    // Use a unique ID based on the sound resource pointer
    std::string voiceId = "direct_" + std::to_string(reinterpret_cast<uintptr_t>(pSoundResource));

    // Get or create the source voice
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);
        auto it = m_SourceVoices.find(voiceId);
        if (it != m_SourceVoices.end() && it->second != nullptr)
        {
            pSourceVoice = it->second;

            // Check if the voice is valid and reset it
            XAUDIO2_VOICE_STATE state;
            try 
            {
                pSourceVoice->GetState(&state);
                pSourceVoice->Stop(0);
                pSourceVoice->FlushSourceBuffers();
            }
            catch (...)
            {
                // If the voice is invalid, create a new one
                pSourceVoice = nullptr;
            }
        }
    }

    // Play the sound
    HRESULT hr = pSoundResource->Play(m_XAudio2.Get(), &pSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    // Update the source voice pool
    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);

        // If we're over the limit, try to clean up non-playing voices
        if (m_SourceVoices.size() >= m_MaxSourceVoices &&
            m_SourceVoices.find(voiceId) == m_SourceVoices.end())
        {
            CleanupSourceVoicePool(false);
        }

        // Store the source voice and update last used time
        m_SourceVoices[voiceId] = pSourceVoice;
        m_VoiceLastUsedTime[voiceId] = std::chrono::steady_clock::now();
    }

    return S_OK;
}

/// <summary>
/// Stop a specific sound
/// </summary>
/// <param name="soundId">ID of the sound to stop</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::StopSound(const std::string& soundId)
{
    if (!m_XAudio2 || !m_MasteringVoice)
        return E_FAIL;

    // Get the source voice
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);
        auto it = m_SourceVoices.find(soundId);

        if (it == m_SourceVoices.end())
            return E_FAIL;

        pSourceVoice = it->second;
    }

    // Get the sound resource from ResourceManager
    auto soundResource = ResourceManager::GetInstance().GetSound(soundId);
    if (!soundResource)
    {
        // If we can't find the resource, just stop the voice directly
        HRESULT hr = pSourceVoice->Stop(0);

        if (FAILED(hr))
            return hr;

        return pSourceVoice->FlushSourceBuffers();
    }

    // Stop the sound using the resource
    return soundResource->Stop(pSourceVoice);
}


/// <summary>
/// Stop all currently playing sounds
/// </summary>
void AudioManager::StopAllSounds()
{
    if (!m_XAudio2 || !m_MasteringVoice)
        return;

    std::lock_guard<std::mutex> lock(m_AudioMutex);

    // First, handle streaming sounds
    std::vector<std::string> streamingSounds;
    for (auto& pair : m_SourceVoices)
    {
        auto soundResource = ResourceManager::GetInstance().GetSound(pair.first);
        if (soundResource && soundResource->IsStreaming())
        {
            streamingSounds.push_back(pair.first);
        }
    }

    // Stop streaming sounds first
    for (const auto& id : streamingSounds)
    {
        auto it = m_SourceVoices.find(id);
        if (it != m_SourceVoices.end() && it->second)
        {
            auto soundResource = ResourceManager::GetInstance().GetSound(id);
            if (soundResource)
            {
                try 
                {
                    soundResource->Stop(it->second);
                }
                catch (...) 
                {
                    // Ignore errors
                }
            }
        }
    }

    // Add a small delay to allow streaming threads to exit
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Then handle all remaining sounds and destroy all voices
    for (auto& pair : m_SourceVoices)
    {
        if (pair.second)
        {
            try 
            {
                pair.second->Stop(0);
                pair.second->FlushSourceBuffers();
                pair.second->DestroyVoice();
            }
            catch (...) 
            {
                // Ignore errors
            }
        }
    }

    // Clear all voice tracking
    m_SourceVoices.clear();
    m_VoiceLastUsedTime.clear();
}

/// <summary>
/// Set the volume for a specific sound
/// </summary>
/// <param name="soundId">ID of the sound</param>
/// <param name="volume">Volume level between 0.0 and 1.0</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::SetSoundVolume(const std::string& soundId, float volume)
{
    if (!m_XAudio2 || !m_MasteringVoice)
        return E_FAIL;

    // Get the source voice
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(m_AudioMutex);
        auto it = m_SourceVoices.find(soundId);
        if (it == m_SourceVoices.end() || !it->second)
            return E_FAIL;

        pSourceVoice = it->second;
    }

    // Clamp volume between 0.0 and 1.0
    volume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;

    // Set the volume directly on the source voice
    return pSourceVoice->SetVolume(volume);
}

/// <summary>
/// Set the master volume affecting all sounds
/// </summary>
/// <param name="volume">Volume level between 0.0 and 1.0</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::SetMasterVolume(float volume)
{
    if (!m_MasteringVoice)
        return E_FAIL;

    // Clamp volume between 0.0 and 1.0
    volume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;

    // Store the master volume
    m_MasterVolume = volume;

    return m_MasteringVoice->SetVolume(volume);
}

/// <summary>
/// Set the audio environment/reverb type
/// </summary>
/// <param name="envType">Environment type enum value</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT AudioManager::SetEnvironment(EnvironmentType envType)
{
    if (!m_XAudio2 || !m_MasteringVoice)
        return E_FAIL;

    // Always start by clearing any existing effect chain
    HRESULT hr = m_MasteringVoice->SetEffectChain(nullptr);
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
    effects[0].OutputChannels = m_Channels;

    effectChain.EffectCount = 1;
    effectChain.pEffectDescriptors = effects;

    // Set effect chain
    hr = m_MasteringVoice->SetEffectChain(&effectChain);
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
    hr = m_MasteringVoice->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters));
    if (FAILED(hr))
    {
        std::cerr << "Failed to set reverb parameters: " << std::hex << hr << std::dec << std::endl;
    }

    // Release XAPO interface
    pXAPO->Release();

    return hr;
}

/// <summary>
/// Configure the source voice pool settings
/// </summary>
/// <param name="maxVoices">Maximum number of voices to maintain in the pool</param>
void AudioManager::ConfigureSourceVoicePool(size_t maxVoices)
{
    std::lock_guard<std::mutex> lock(m_AudioMutex);

    m_MaxSourceVoices = maxVoices;
    std::cout << "Source voice pool configured: max=" << maxVoices << std::endl;

    // Clean up if we're over the new limit
    if (m_SourceVoices.size() > m_MaxSourceVoices)
    {
        CleanupSourceVoicePool();
    }
}

/// <summary>
/// Clean up source voices based on LRU policy when needed
/// </summary>
/// <param name="forceCleanup">If true, forces cleanup even if not over the limit</param>
/// <summary>
/// Clean up source voices based on LRU policy when needed
/// </summary>
/// <param name="forceCleanup">If true, forces cleanup even if not over the limit</param>
void AudioManager::CleanupSourceVoicePool(bool forceCleanup)
{
    std::lock_guard<std::mutex> lock(m_AudioMutex);

    // If we're not over the limit and not forcing cleanup, there's nothing to do
    if (m_SourceVoices.size() <= m_MaxSourceVoices && !forceCleanup)
        return;

    // Calculate how many voices to remove, using proper signed arithmetic
    size_t targetCount = 0;
    if (m_SourceVoices.size() > m_MaxSourceVoices) 
    {
        targetCount = m_SourceVoices.size() - m_MaxSourceVoices;
    }
    else if (forceCleanup) 
    {
        // If forcing cleanup and not over limit, remove at least one voice
        targetCount = 1;
    }
    else 
    {
        // Nothing to do
        return;
    }

    // Get current time
    auto currentTime = std::chrono::steady_clock::now();

    // Collect voice information for sorting
    std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> voices;
    voices.reserve(m_SourceVoices.size());

    for (const auto& pair : m_SourceVoices)
    {
        // Skip voices that are currently playing
        XAUDIO2_VOICE_STATE state;
        try 
        {
            pair.second->GetState(&state);
            if (state.BuffersQueued > 0)
                continue;
        }
        catch (...) 
        {
            // If we can't get the state, assume it's invalid and should be cleaned up
        }

        auto lastUsedIter = m_VoiceLastUsedTime.find(pair.first);
        auto lastUsedTime = (lastUsedIter != m_VoiceLastUsedTime.end())
            ? lastUsedIter->second
            : std::chrono::steady_clock::time_point();

        voices.push_back(std::make_pair(pair.first, lastUsedTime));
    }

    // Sort voices by last used time (oldest first)
    std::sort(voices.begin(), voices.end(),
        [](const auto& a, const auto& b) 
        {
            return a.second < b.second;
        });

    // Determine how many we can safely remove
    size_t canRemove = targetCount;
    if (canRemove > voices.size())
        canRemove = voices.size();

    // If we can't remove enough, just remove what we can
    if (canRemove < targetCount)
    {
        std::cout << "Warning: Can only remove " << canRemove
            << " of " << targetCount << " source voices needed for cleanup" << std::endl;
    }

    // Remove the oldest unused voices
    size_t removedCount = 0;
    for (size_t i = 0; i < canRemove; ++i)
    {
        const std::string& voiceId = voices[i].first;
        auto it = m_SourceVoices.find(voiceId);
        if (it != m_SourceVoices.end())
        {
            try 
            {
                // Destroy the voice
                it->second->DestroyVoice();
            }
            catch (...) 
            {
                // Ignore errors if voice is already invalid
            }
            // Remove from our maps
            m_SourceVoices.erase(it);
            m_VoiceLastUsedTime.erase(voiceId);
            removedCount++;
        }
    }

    if (removedCount > 0)
    {
        std::cout << "Source voice pool cleanup: removed " << removedCount
            << " voices, " << m_SourceVoices.size() << " remaining" << std::endl;
    }
}

/// <summary>
/// Check if a specific voice is currently playing
/// </summary>
/// <param name="soundId">ID of the sound to check</param>
/// <returns>True if the voice is playing, false otherwise</returns>
bool AudioManager::IsVoicePlaying(const std::string& soundId)
{
    std::lock_guard<std::mutex> lock(m_AudioMutex);

    auto it = m_SourceVoices.find(soundId);
    if (it != m_SourceVoices.end() && it->second)
    {
        try 
        {
            XAUDIO2_VOICE_STATE state;
            it->second->GetState(&state);
            return (state.BuffersQueued > 0);
        }
        catch (...) 
        {
            // If we can't get the state, assume it's not playing
        }
    }

    return false;
}

/// <summary>
/// Get the current status of the voice pool
/// </summary>
/// <param name="totalVoices">Will be populated with the total number of voices</param>
/// <param name="playingVoices">Will be populated with the number of currently playing voices</param>
void AudioManager::GetVoicePoolStatus(size_t& totalVoices, size_t& playingVoices)
{
    std::lock_guard<std::mutex> lock(m_AudioMutex);

    totalVoices = m_SourceVoices.size();
    playingVoices = 0;

    for (const auto& pair : m_SourceVoices)
    {
        if (pair.second)
        {
            try 
            {
                XAUDIO2_VOICE_STATE state;
                pair.second->GetState(&state);
                if (state.BuffersQueued > 0)
                {
                    playingVoices++;
                }
            }
            catch (...) 
            {
                // Ignore errors when checking state
            }
        }
    }
}

/// <summary>
/// Perform periodic maintenance on the voice pool
/// </summary>
void AudioManager::PerformMaintenance()
{
    // Get current status
    size_t totalVoices, playingVoices;
    GetVoicePoolStatus(totalVoices, playingVoices);

    // If we have many idle voices, consider cleaning some up
    if (totalVoices > m_MaxSourceVoices / 2 &&
        playingVoices < totalVoices / 4)
    {
        CleanupSourceVoicePool(true);
    }
}