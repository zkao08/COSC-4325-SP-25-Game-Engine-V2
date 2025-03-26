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
#include <xaudio2.h>
#include <x3daudio.h>
#include <iostream>
#include <comdef.h>
#include <Windows.h>
#include <filesystem>

//little endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

//global singleton
AudioManager gAudioManager;

AudioManager::AudioManager() : pXAudio2(nullptr), pMasteringVoice(nullptr)
{
	//do nothing
}

AudioManager::~AudioManager()
{
	//do nothing
}

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

void AudioManager::shutDown()
{
    // Clean up source voices in the pool
    while (!sourceVoicePool.empty()) {
        IXAudio2SourceVoice* pSourceVoice = sourceVoicePool.front();
        sourceVoicePool.pop();
        pSourceVoice->DestroyVoice();
    }

    // Destroy the mastering voice
    if (pMasteringVoice) {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }

    // Release XAudio2 engine
    if (pXAudio2) {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }

    // Uninitialize COM
    CoUninitialize();
    std::cout << "AudioManager shut down." << std::endl;
}

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

HRESULT AudioManager::LoadAudioFile(const std::wstring& filePath, XAUDIO2_BUFFER& buffer, WAVEFORMATEXTENSIBLE& wfx)
{
    // Ensure file path is absolute
    std::wstring fullPath = GetProjectRoot() + L"\\" + filePath;

    // Open file using wide-character API
    HANDLE hFile = CreateFileW(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD errorCode = GetLastError();
        wprintf(L"Failed to open file: %s, Error Code: %d\n", fullPath.c_str(), errorCode);
        return HRESULT_FROM_WIN32(errorCode);
    }

    DWORD dwChunkSize = 0;
    DWORD dwChunkDataPosition = 0;

    // Find the 'RIFF' chunk
    HRESULT hr = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) return hr;

    DWORD filetype;
    hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkDataPosition);
    if (FAILED(hr) || filetype != fourccWAVE)
        return E_FAIL;

    // Find 'fmt ' chunk
    hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) return hr;

    hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) return hr;

    // Find 'data' chunk
    hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) return hr;

    std::unique_ptr<BYTE[]> pDataBuffer = std::make_unique<BYTE[]>(dwChunkSize);
    hr = ReadChunkData(hFile, pDataBuffer.get(), dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) return hr;

    buffer.AudioBytes = dwChunkSize;
    buffer.pAudioData = pDataBuffer.get();
    buffer.Flags = XAUDIO2_END_OF_STREAM; // End of stream flag

    CloseHandle(hFile);
    return S_OK;
}

HRESULT AudioManager::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize)
            return S_FALSE;
    }

    return S_OK;
}

HRESULT AudioManager::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

HRESULT AudioManager::PlaySound(const std::string& filePath, bool reuse)
{
    // Convert std::string to std::wstring
    std::wstring wFilePath(filePath.begin(), filePath.end());

    XAUDIO2_BUFFER buffer = { 0 };
    WAVEFORMATEXTENSIBLE wfx = { 0 };

    // Check if the sound is already cached
    if (reuse && m_cachedAudio.find(wFilePath) != m_cachedAudio.end()) 
    {
        buffer = m_cachedAudio[wFilePath].first;
        wfx = m_cachedAudio[wFilePath].second;
    }
    else 
    {
        // Load the sound from the file
        HRESULT hr = LoadAudioFile(wFilePath, buffer, wfx);
        if (FAILED(hr)) return hr;

        // Cache it if it's a frequently used sound
        if (reuse) 
        {
            m_cachedAudio[wFilePath] = { buffer, wfx };
        }
    }

    // Check if there is a free source voice in the pool
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    if (!sourceVoicePool.empty()) 
    {
        pSourceVoice = sourceVoicePool.front();
        sourceVoicePool.pop();
    }
    else 
    {
        // If no voice in the pool, create a new one
        HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx);
        if (FAILED(hr)) return hr;
    }

    // Submit the buffer to the source voice and start playback
    HRESULT hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) return hr;

    hr = pSourceVoice->Start(0);
    if (FAILED(hr)) return hr;

    // Return the voice to the pool when done (manual cleanup)
    sourceVoicePool.push(pSourceVoice);

    return S_OK;
}

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

void AudioManager::DestroySourceVoice(IXAudio2SourceVoice* pSourceVoice)
{
    // Destroy the source voice (free its resources)
    pSourceVoice->DestroyVoice();
}