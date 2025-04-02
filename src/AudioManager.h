#pragma once
#include <xaudio2.h>
#include <x3daudio.h>
#include <iostream>
#include <comdef.h>
#include <Windows.h>
#include <queue>
#include <string>
#include <unordered_map>
#include <memory>
#include "SoundResource.h"

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    bool startUp();
    void shutDown();

    // Plays a sound effect from a file path, optionally reusing the sound if it's frequently used
    HRESULT PlaySound(const std::string& filePath, bool reuse);

    // Load a sound resource without playing it immediately
    std::shared_ptr<SoundResource> LoadSound(const std::string& filePath, bool cache = true);

    // Play a preloaded sound resource
    HRESULT PlaySoundResource(std::shared_ptr<SoundResource> soundResource);

    // Get a sound resource by name (useful for game assets referenced by name)
    std::shared_ptr<SoundResource> GetSound(const std::string& filePath);

    void Update(); // To manually check for cleanup
    void DestroySourceVoice(IXAudio2SourceVoice* pSourceVoice);

    // Get the XAudio2 engine
    IXAudio2* GetXAudio2Engine() const { return pXAudio2; }

private:
    // Helper function to create a source voice
    HRESULT CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pWaveFormat);

    IXAudio2* pXAudio2;                     // Pointer to the XAudio2 engine
    IXAudio2MasteringVoice* pMasteringVoice; // Mastering voice for the audio output
    std::queue<IXAudio2SourceVoice*> sourceVoicePool; // Pool of available source voices
    const size_t MAX_POOL_SIZE = 10; // Max number of voices in the pool

    // Cached sound resources for frequently used sounds
    std::unordered_map<std::string, std::shared_ptr<SoundResource>> m_cachedSounds;
};

// Global singleton
extern AudioManager gAudioManager;

// Helper function
std::wstring GetProjectRoot();

#endif