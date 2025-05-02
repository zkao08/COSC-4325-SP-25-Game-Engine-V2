// AudioManager.h
#pragma once
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <chrono>

// Forward declarations
class SoundResource;

// Room/Environment types for sound effects
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
    // Singleton access
    static AudioManager& GetInstance();

    // Delete copy constructor and assignment operator
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Core system methods
    bool startUp();
    void shutDown();

    // Playback methods
    HRESULT PlaySound(const std::string& soundId, float volume = 1.0f);
    HRESULT PlaySoundDirect(SoundResource* pSoundResource, float volume = 1.0f);
    HRESULT StopSound(const std::string& soundId);
    void StopAllSounds();

    // Volume control
    HRESULT SetSoundVolume(const std::string& soundId, float volume);
    HRESULT SetMasterVolume(float volume);
    float GetMasterVolume() const { return m_MasterVolume; }

    // Effects
    HRESULT SetEnvironment(EnvironmentType envType);

    // Engine access
    IXAudio2* GetXAudio2Engine() const { return m_XAudio2.Get(); }

    // Source voice pool management
    void ConfigureSourceVoicePool(size_t maxVoices);
    void CleanupSourceVoicePool(bool forceCleanup = false);
    bool IsVoicePlaying(const std::string& soundId);
    void GetVoicePoolStatus(size_t& totalVoices, size_t& playingVoices);
    void PerformMaintenance();

private:
    AudioManager();
    ~AudioManager();

    // XAudio2 interfaces
    Microsoft::WRL::ComPtr<IXAudio2> m_XAudio2;
    IXAudio2MasteringVoice* m_MasteringVoice;

    // Source voices - mapping from sound ID to voice
    std::unordered_map<std::string, IXAudio2SourceVoice*> m_SourceVoices;

    // Voice pool management
    size_t m_MaxSourceVoices = 64;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_VoiceLastUsedTime;

    // Master volume level
    float m_MasterVolume;

    // Cached speaker configuration
    DWORD m_ChannelMask;
    UINT32 m_Channels;

    // Thread safety
    std::mutex m_AudioMutex;
};