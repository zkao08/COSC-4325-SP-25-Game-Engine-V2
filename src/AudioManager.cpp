/// <file>
/// <summary>
/// Audio Manager using XAudio2 and X3DAudio 
/// for the audio engine subsystem of the game engine
/// Provides a public interface for playing, stopping, and managing audio resources.
/// </summary>
/// 
/// References:
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-apis-portal
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/x3daudio
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-3 </date>
/// </file>

#include "AudioManager.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <algorithm>

// Define global singleton accessor
AudioManager& gAudioManager = AudioManager::GetInstance();

AudioManager::AudioManager()
    : pXAudio2(nullptr),
    pMasteringVoice(nullptr),
    masterVolume(1.0f)
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
bool AudioManager::startUp()
{
    // Initialize COM (if not already done)
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != S_FALSE && hr != RPC_E_CHANGED_MODE) {
        std::cerr << "Failed to initialize COM: " << std::hex << hr << std::dec << std::endl;
        return false;
    }

    // Create XAudio2 instance
    hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        std::cerr << "Failed to create XAudio2 instance: " << std::hex << hr << std::dec << std::endl;
        CoUninitialize();
        return false;
    }

    // Create mastering voice
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr)) {
        std::cerr << "Failed to create mastering voice: " << std::hex << hr << std::dec << std::endl;
        pXAudio2.Reset();
        CoUninitialize();
        return false;
    }

    // Set initial master volume
    pMasteringVoice->SetVolume(masterVolume);

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

        for (auto& pair : sourceVoices) {
            if (pair.second) {
                pair.second->DestroyVoice();
                pair.second = nullptr;
            }
        }
        sourceVoices.clear();
        soundResources.clear();
    }

    // Release XAudio2 resources
    if (pMasteringVoice) {
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
HRESULT AudioManager::PlaySound(const std::string& filePath, bool isSoundEffect, float volume)
{
    if (!pXAudio2 || !pMasteringVoice)
        return E_FAIL;

    std::wstring wFilePath = ConvertToWideString(filePath);

    // Get or load the sound resource
    std::shared_ptr<SoundResource> resource = GetOrLoadResource(wFilePath, isSoundEffect);
    if (!resource)
        return E_FAIL;

    // Get the source voice if it exists, or create a new one
    IXAudio2SourceVoice* pSourceVoice = nullptr;

    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        auto it = sourceVoices.find(wFilePath);
        if (it != sourceVoices.end()) {
            pSourceVoice = it->second;

            // Stop the voice before reusing it
            pSourceVoice->Stop(0);
            pSourceVoice->FlushSourceBuffers();
        }
    }

    // Play the sound with volume control
    HRESULT hr = resource->Play(pXAudio2.Get(), &pSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    // Store the source voice
    {
        std::lock_guard<std::mutex> lock(resourceMutex);
        sourceVoices[wFilePath] = pSourceVoice;
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
    auto resourceIt = soundResources.find(wFilePath);
    if (resourceIt == soundResources.end())
        return E_FAIL;

    // Find the source voice
    auto voiceIt = sourceVoices.find(wFilePath);
    if (voiceIt == sourceVoices.end())
        return E_FAIL;

    // Stop the sound
    return resourceIt->second->Stop(voiceIt->second);
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
    for (auto& pair : soundResources) {
        auto voiceIt = sourceVoices.find(pair.first);
        if (voiceIt != sourceVoices.end() && voiceIt->second) {
            pair.second->Stop(voiceIt->second);
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
    auto voiceIt = sourceVoices.find(wFilePath);
    if (voiceIt == sourceVoices.end() || !voiceIt->second)
        return E_FAIL;

    // Find the resource
    auto resourceIt = soundResources.find(wFilePath);
    if (resourceIt == soundResources.end())
        return E_FAIL;

    // Set volume
    return resourceIt->second->SetVolume(voiceIt->second, volume);
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
    if (volume < 0.0f) {
        volume = 0.0f;
    }
    else if (volume > 1.0f) {
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
std::shared_ptr<SoundResource> AudioManager::GetOrLoadResource(const std::wstring& filePath, bool isSoundEffect)
{
    std::lock_guard<std::mutex> lock(resourceMutex);

    // Check if resource already exists
    auto it = soundResources.find(filePath);
    if (it != soundResources.end())
        return it->second;

    // Create new resource
    auto resource = std::make_shared<SoundResource>();

    // Select resource type based on sound type
    SoundResource::ResourceType resourceType = isSoundEffect ?
        SoundResource::SOUND_EFFECT : SoundResource::STREAMING;

    // Load the resource
    HRESULT hr = resource->Load(filePath, resourceType);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to load sound resource: " << filePath << std::endl;
        return nullptr;
    }

    // Store and return the resource
    soundResources[filePath] = resource;
    return resource;
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

/// <summary>
/// Helper function to get the project root directory.
/// </summary>
/// <returns></returns>
std::wstring AudioManager::GetProjectRoot()
{
    // Get current working directory
    wchar_t buffer[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, buffer);

    // Remove "\build" from the path if it exists
    std::wstring path(buffer);
    std::wstring buildDir = L"\\build";
    size_t pos = path.find(buildDir);
    if (pos != std::wstring::npos) {
        path = path.substr(0, pos);
    }

    return path;
}

// For use by SoundResource.cpp
std::wstring GetProjectRoot()
{
    return gAudioManager.GetProjectRoot();
}