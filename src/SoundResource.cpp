/// <file>
/// <summary>
/// Sound Resource class for loading and managing sound files.
/// Encapsulates and models the sound resource for the audio engine.
/// </summary>
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-3 </date>
/// </file>

#include "SoundResource.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>

// Forward declaration of GetProjectRoot from AudioManager
std::wstring GetProjectRoot();

/// <summary>
/// Initializes the SoundResource object.
/// </summary>
SoundResource::SoundResource()
    : resourceType(SOUND_EFFECT),
    isStreaming(false),
    streamingFileHandle(INVALID_HANDLE_VALUE),
    dataChunkSize(0),
    dataChunkPosition(0)
{
    ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
}

/// <summary>
/// Cleans up the SoundResource object.
/// </summary>
SoundResource::~SoundResource()
{
    // Stop streaming if active
    StopStreaming();

    // Close file handle if open
    if (streamingFileHandle != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(streamingFileHandle);
        streamingFileHandle = INVALID_HANDLE_VALUE;
    }
}

/// <summary>
/// Loads a sound file from the specified path into the buffer.
/// </summary>
/// <param name="filePath"></param>
/// <param name="type"></param>
/// <returns></returns>
HRESULT SoundResource::Load(const std::wstring& filePath, ResourceType type)
{
    this->filePath = filePath;
    this->resourceType = type;

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
    if (FAILED(hr)) 
    {
        CloseHandle(hFile);
        return hr;
    }

    DWORD filetype;
    hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkDataPosition);
    if (FAILED(hr) || filetype != fourccWAVE) 
    {
        CloseHandle(hFile);
        return hr;
    }

    // Find 'fmt ' chunk
    hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) 
    {
        CloseHandle(hFile);
        return hr;
    }

    hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) 
    {
        CloseHandle(hFile);
        return hr;
    }

    // Find 'data' chunk
    hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkDataPosition);
    if (FAILED(hr)) 
    {
        CloseHandle(hFile);
        return hr;
    }

    // Store data chunk info for streaming
    dataChunkSize = dwChunkSize;
    dataChunkPosition = dwChunkDataPosition;

    if (resourceType == SOUND_EFFECT) 
    {
        // For sound effects, load the entire file into memory
        audioData.resize(dwChunkSize);

        // Read the data into our vector
        hr = ReadChunkData(hFile, audioData.data(), dwChunkSize, dwChunkDataPosition);
        if (FAILED(hr)) 
        {
            CloseHandle(hFile);
            return hr;
        }

        // Set up the audio buffer
        buffer.AudioBytes = dwChunkSize;
        buffer.pAudioData = audioData.data();  // Points to vector memory which is managed automatically
        buffer.Flags = XAUDIO2_END_OF_STREAM;  // End of stream flag

        CloseHandle(hFile);
    }
    else 
    {
        // For streaming audio, keep the file handle open
        streamingFileHandle = hFile;

        // We'll load data in chunks during playback
        buffer.AudioBytes = 0;
        buffer.pAudioData = nullptr;
    }

    return S_OK;
}

/// <summary>
/// Play the sound resource.
/// </summary>
/// <param name="pXAudio2"></param>
/// <param name="ppSourceVoice"></param>
/// <param name="volume"></param>
/// <returns></returns>
HRESULT SoundResource::Play(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice, float volume)
{
    if (!pXAudio2 || !ppSourceVoice)
        return E_INVALIDARG;

    // Create a source voice if one wasn't provided
    if (*ppSourceVoice == nullptr) 
    {
        HRESULT hr = pXAudio2->CreateSourceVoice(ppSourceVoice, (WAVEFORMATEX*)&wfx);
        if (FAILED(hr))
            return hr;
    }

    // Set the volume
    HRESULT hr = SetVolume(*ppSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    if (resourceType == SOUND_EFFECT) 
    {
        // For sound effects, just submit the buffer and play
        hr = (*ppSourceVoice)->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
            return hr;

        return (*ppSourceVoice)->Start(0);
    }
    else {
        // For streaming audio, start the streaming process
        return StartStreaming(*ppSourceVoice);
    }
}

/// <summary>
/// Stop the sound resource.
/// </summary>
/// <param name="pSourceVoice"></param>
/// <returns></returns>
HRESULT SoundResource::Stop(IXAudio2SourceVoice* pSourceVoice)
{
    if (!pSourceVoice)
        return E_INVALIDARG;

    // Stop streaming if needed
    if (resourceType == STREAMING) 
    {
        StopStreaming();
    }

    // Stop the source voice
    HRESULT hr = pSourceVoice->Stop(0);
    if (FAILED(hr))
        return hr;

    // Flush remaining buffers
    return pSourceVoice->FlushSourceBuffers();
}

/// <summary>
/// Set the volume for the source voice.
/// </summary>
/// <param name="pSourceVoice"></param>
/// <param name="volume"></param>
/// <returns></returns>
HRESULT SoundResource::SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume)
{
    if (!pSourceVoice)
        return E_INVALIDARG;

    // Clamp volume between 0.0 and 1.0
    if (volume < 0.0f) 
    {
        volume = 0.0f;
    }
    else if (volume > 1.0f)
    {
        volume = 1.0f;
    }


    return pSourceVoice->SetVolume(volume);
}

/// <summary>
/// Plays the streaming audio.
/// </summary>
/// <param name="pSourceVoice"></param>
/// <returns></returns>
HRESULT SoundResource::StartStreaming(IXAudio2SourceVoice* pSourceVoice)
{
    if (streamingFileHandle == INVALID_HANDLE_VALUE)
        return E_FAIL;

    // Stop any existing streaming
    StopStreaming();

    // Reset file position to the beginning of the data chunk
    SetFilePointer(streamingFileHandle, dataChunkPosition, NULL, FILE_BEGIN);

    // Start streaming thread
    isStreaming = true;
    streamingThread = std::thread(&SoundResource::StreamingWorker, this, pSourceVoice);

    // Start playback
    return pSourceVoice->Start(0);
}

/// <summary>
/// Stops the streaming audio.
/// </summary>
void SoundResource::StopStreaming()
{
    if (isStreaming)
    {
        isStreaming = false;

        // Wait for streaming thread to finish
        if (streamingThread.joinable()) 
        {
            streamingThread.join();
        }
    }
}

/// <summary>
/// Worker function for streaming audio.
/// </summary>
/// <param name="pSourceVoice"></param>
void SoundResource::StreamingWorker(IXAudio2SourceVoice* pSourceVoice)
{
    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;
    std::vector<BYTE> streamBuffer(STREAMING_BUFFER_SIZE);
    XAUDIO2_BUFFER audioBuffer = {};
    XAUDIO2_VOICE_STATE voiceState = {};

    while (isStreaming && totalBytesRead < dataChunkSize) 
    {
        // Lock to ensure thread safety when accessing file handle
        std::lock_guard<std::mutex> lock(streamingMutex);

        // Calculate bytes to read
        DWORD bytesToRead;
        if (STREAMING_BUFFER_SIZE < (dataChunkSize - totalBytesRead)) 
        {
            bytesToRead = static_cast<DWORD>(STREAMING_BUFFER_SIZE);
        }
        else
        {
            bytesToRead = static_cast<DWORD>(dataChunkSize - totalBytesRead);
        }

        // Read next chunk
        if (!ReadFile(streamingFileHandle, streamBuffer.data(), bytesToRead, &bytesRead, nullptr) || bytesRead == 0) 
        {
            // Error or end of file
            break;
        }

        // Update total bytes read
        totalBytesRead += bytesRead;

        // Set up buffer
        audioBuffer.AudioBytes = bytesRead;
        audioBuffer.pAudioData = streamBuffer.data();
        audioBuffer.Flags = (totalBytesRead >= dataChunkSize) ? XAUDIO2_END_OF_STREAM : 0;

        // Check if we need to wait for buffers to free up
        pSourceVoice->GetState(&voiceState);
        while (voiceState.BuffersQueued >= 3) // Maximum of 3 buffers in queue
        { 
            if (!isStreaming) return;  // Check if we should exit
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            pSourceVoice->GetState(&voiceState);
        }

        // Submit buffer
        if (FAILED(pSourceVoice->SubmitSourceBuffer(&audioBuffer))) {
            break;
        }

        // If we've reached the end and we're still streaming, loop back to start
        if (totalBytesRead >= dataChunkSize && isStreaming) {
            totalBytesRead = 0;
            SetFilePointer(streamingFileHandle, dataChunkPosition, NULL, FILE_BEGIN);
        }
    }
}

/// <summary>
/// Finds a chunk in the file.
/// </summary>
/// <param name="hFile"></param>
/// <param name="fourcc"></param>
/// <param name="dwChunkSize"></param>
/// <param name="dwChunkDataPosition"></param>
/// <returns></returns>
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

/// <summary>
/// Reads chunk data from the file.
/// </summary>
/// <param name="hFile"></param>
/// <param name="buffer"></param>
/// <param name="buffersize"></param>
/// <param name="bufferoffset"></param>
/// <returns></returns>
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