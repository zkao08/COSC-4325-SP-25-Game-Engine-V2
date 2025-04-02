// SoundResource.cpp
#include "SoundResource.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>

// Forward declaration of GetProjectRoot from AudioManager
std::wstring GetProjectRoot();

SoundResource::SoundResource()
{
    ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
}

SoundResource::~SoundResource()
{
    // No need to manually clean up audioData as std::vector handles it automatically
}

HRESULT SoundResource::Load(const std::wstring& filePath)
{
    this->filePath = filePath;
    
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
    if (FAILED(hr)) {
        CloseHandle(hFile);
        return hr;
    }

    DWORD filetype;
    hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkDataPosition);
    if (FAILED(hr) || filetype != fourccWAVE) {
        CloseHandle(hFile);
        return E_FAIL;
    }

    // Find 'fmt ' chunk
    hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) {
        CloseHandle(hFile);
        return hr;
    }

    hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) {
        CloseHandle(hFile);
        return hr;
    }

    // Find 'data' chunk
    hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) {
        CloseHandle(hFile);
        return hr;
    }

    // Resize the vector to hold the audio data
    audioData.resize(dwChunkSize);
    
    // Read the data into our vector
    hr = ReadChunkData(hFile, audioData.data(), dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) {
        CloseHandle(hFile);
        return hr;
    }

    // Set up the audio buffer
    buffer.AudioBytes = dwChunkSize;
    buffer.pAudioData = audioData.data();  // Points to vector memory which is managed automatically
    buffer.Flags = XAUDIO2_END_OF_STREAM;  // End of stream flag

    CloseHandle(hFile);
    return S_OK;
}

HRESULT SoundResource::Play(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice)
{
    if (!pXAudio2 || !ppSourceVoice)
        return E_INVALIDARG;

    // Create a source voice if one wasn't provided
    if (*ppSourceVoice == nullptr) {
        HRESULT hr = pXAudio2->CreateSourceVoice(ppSourceVoice, (WAVEFORMATEX*)&wfx);
        if (FAILED(hr))
            return hr;
    }
    
    // Submit buffer & play
    HRESULT hr = (*ppSourceVoice)->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
        return hr;
    
    return (*ppSourceVoice)->Start(0);
}

HRESULT SoundResource::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
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

HRESULT SoundResource::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}