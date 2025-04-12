// AudioManager.h
#pragma once
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <wrl/client.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include "SoundResource.h"

/// <summary>
/// Room/Environment types for sound effects
/// </summary>
enum EnvironmentType 
{
    ENV_NORMAL,
    ENV_CAVE,
    ENV_UNDERWATER,
    ENV_LARGE_HALL
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
        bool startUp();

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

        // Convert path string from narrow to wide
        std::wstring ConvertToWideString(const std::string& str);

        // Get project root directory
        std::wstring GetProjectRoot();

        // Room/Environment Effects
        HRESULT SetEnvironment(EnvironmentType envType);

    private:
        // XAudio2 interfaces
        Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
        IXAudio2MasteringVoice* pMasteringVoice;

        // Sound resources and source voices
        std::unordered_map<std::wstring, std::shared_ptr<SoundResource>> soundResources;
        std::unordered_map<std::wstring, IXAudio2SourceVoice*> sourceVoices;

        // Mutex for thread safety
        std::mutex resourceMutex;

        // Master volume level
        float masterVolume;

        // Helper methods
        std::shared_ptr<SoundResource> GetOrLoadResource(const std::wstring& filePath, bool isSoundEffect);

        // Cached speaker configuration
        DWORD channelMask;
        UINT32 channels;
};

// Global singleton accessor
extern AudioManager& gAudioManager;