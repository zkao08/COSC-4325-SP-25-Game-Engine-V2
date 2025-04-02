/// <file>
/// <summary>
/// Audio Manager using XAudio2 and X3DAudio 
/// for the audio engine subsystem of the game engine
/// </summary>
/// 
/// References:
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-apis-portal
/// https://learn.microsoft.com/en-us/windows/win32/xaudio2/x3daudio
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-3-21 </date>

#include "AudioManager.h"
#include "SoundResource.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <iostream>
#include <comdef.h>
#include <Windows.h>
#include <filesystem>

//global singleton
AudioManager gAudioManager;

/// <summary>
/// Constructor/deconstructor do nothing as per brute-force design
/// </summary>
AudioManager::AudioManager() : pXAudio2(nullptr), pMasteringVoice(nullptr)
{
    //do nothing
}

/// <summary>
/// Constructor/deconstructor do nothing as per brute-force design
/// </summary>
AudioManager::~AudioManager()
{
    //do nothing
}

/// <summary>
/// Start up the audio manager, initialize XAudio2
/// </summary>
/// <returns>TRUE if successful</returns>
bool AudioManager::startUp()
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        std::cerr << "Failed to initialize COM: " << _com_error(hr).ErrorMessage() << std::endl;
        return false;
    }

    // Create the XAudio2 engine
    hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create XAudio2 engine: " << _com_error(hr).ErrorMessage() << std::endl;
        CoUninitialize(); // Clean up COM if initialization failed
        return false;
    }

    // Create the mastering voice
    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create mastering voice: " << _com_error(hr).ErrorMessage() << std::endl;
        pXAudio2->Release();
        pXAudio2 = nullptr;
        CoUninitialize();
        return false;
    }

    std::cout << "AudioManager started up successfully!" << std::endl;
    return true;
}

/// <summary>
/// Shut down audio manager, clean up resources
/// </summary>
void AudioManager::shutDown()
{
    // Clear the cached sounds - this will delete all the sound resources
    m_cachedSounds.clear();

    // Clean up source voices in the pool
    while (!sourceVoicePool.empty()) 
    {
        IXAudio2SourceVoice* pSourceVoice = sourceVoicePool.front();
        sourceVoicePool.pop();
        pSourceVoice->DestroyVoice();
    }

    // Destroy the mastering voice
    if (pMasteringVoice) 
    {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }

    // Release XAudio2 engine
    if (pXAudio2) 
    {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }

    // Uninitialize COM
    CoUninitialize();
    std::cout << "AudioManager shut down." << std::endl;
}

/// <summary>
/// Helper function to get the root file path of the project
/// </summary>
/// <returns></returns>
std::wstring GetProjectRoot()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);

    // Use std::filesystem to remove the file specification (executable name)
    std::filesystem::path exePath(path);
    std::filesystem::path rootPath = exePath.parent_path(); // Removes the executable name

    // Find and trim the build directory if present
    auto pos = rootPath.wstring().find(L"\\build");
    if (pos != std::wstring::npos)
    {
        rootPath = rootPath.wstring().substr(0, pos);
    }

    return rootPath.wstring();
}

/// <summary>
/// Create a source voice
/// </summary>
HRESULT AudioManager::CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pWaveFormat)
{
    if (!pXAudio2 || !ppSourceVoice || !pWaveFormat)
        return E_INVALIDARG;

    // Check if there is a free source voice in the pool
    if (!sourceVoicePool.empty())
    {
        *ppSourceVoice = sourceVoicePool.front();
        sourceVoicePool.pop();
        return S_OK;
    }

    // If no voice in the pool, create a new one
    return pXAudio2->CreateSourceVoice(ppSourceVoice, pWaveFormat);
}

/// <summary>
/// Load a sound resource 
/// </summary>
std::shared_ptr<SoundResource> AudioManager::LoadSound(const std::string& filePath, bool cache)
{
    // Check if the sound is already cached
    auto it = m_cachedSounds.find(filePath);
    if (it != m_cachedSounds.end())
    {
        return it->second;
    }

    // Convert std::string to std::wstring
    std::wstring wFilePath(filePath.begin(), filePath.end());

    // Create a new sound resource
    std::shared_ptr<SoundResource> soundResource = std::make_shared<SoundResource>();

    // Load the sound file
    HRESULT hr = soundResource->Load(wFilePath);
    if (FAILED(hr))
    {
        std::cerr << "Failed to load sound file: " << filePath << std::endl;
        return nullptr;
    }

    // Cache it if requested
    if (cache)
    {
        m_cachedSounds[filePath] = soundResource;
    }

    return soundResource;
}

/// <summary>
/// Get a cached sound resource
/// </summary>
std::shared_ptr<SoundResource> AudioManager::GetSound(const std::string& filePath)
{
    auto it = m_cachedSounds.find(filePath);
    if (it != m_cachedSounds.end())
    {
        return it->second;
    }
    return nullptr;
}

/// <summary>
/// Play a previously loaded sound resource
/// </summary>
HRESULT AudioManager::PlaySoundResource(std::shared_ptr<SoundResource> soundResource)
{
    if (!soundResource)
        return E_INVALIDARG;

    // Get or create a source voice
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    HRESULT hr = CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)soundResource->GetWaveFormat());
    if (FAILED(hr))
        return hr;

    // Play the sound
    hr = soundResource->Play(pXAudio2, &pSourceVoice);
    if (FAILED(hr))
    {
        // Return voice to pool if play failed
        sourceVoicePool.push(pSourceVoice);
        return hr;
    }

    // For now, we'll return the voice to the pool after it's started
    // In a real implementation, you might want to use callbacks to know when it's finished
    sourceVoicePool.push(pSourceVoice);

    return S_OK;
}

/// <summary>
/// Public interface to play an audio file.
/// </summary>
/// <param name="filePath">Filepath to the audio file</param>
/// <param name="reuse">TRUE if this sound will be used frequently</param>
/// <returns></returns>
HRESULT AudioManager::PlaySound(const std::string& filePath, bool reuse)
{
    // Load the sound resource (this will return a cached version if available)
    std::shared_ptr<SoundResource> soundResource = LoadSound(filePath, reuse);
    if (!soundResource)
        return E_FAIL;

    // Play the sound resource
    return PlaySoundResource(soundResource);
}

/// <summary>
/// Call to periodically clean up resources
/// </summary>
void AudioManager::Update()
{
    // Periodically check for finished voices and clean them up
    size_t poolSize = sourceVoicePool.size();
    if (poolSize > MAX_POOL_SIZE)
    {
        // Limit the number of voices in the pool
        // Remove the oldest or least used source voices
        IXAudio2SourceVoice* pSourceVoice = sourceVoicePool.front();
        if (pSourceVoice)
        {
            sourceVoicePool.pop();
            DestroySourceVoice(pSourceVoice);
        }
    }
}

/// <summary>
/// Helper function to clean up resources
/// </summary>
/// <param name="pSourceVoice"></param>
void AudioManager::DestroySourceVoice(IXAudio2SourceVoice* pSourceVoice)
{
    // Destroy the source voice (free its resources)
    pSourceVoice->DestroyVoice();
}