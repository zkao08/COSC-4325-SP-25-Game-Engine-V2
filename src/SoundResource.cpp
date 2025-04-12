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
    if (!pSourceVoice)
        return;

    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;

    // Get the wave format block alignment to ensure proper buffer sizes
    const WAVEFORMATEX* pFormat = reinterpret_cast<const WAVEFORMATEX*>(&wfx);
    DWORD blockAlign = pFormat->nBlockAlign;

    // Ensure buffer size is large enough and a multiple of the block alignment
    // Using a larger buffer size to prevent underruns
    size_t bufferMultiplier = 4;  // Increased from default
    size_t bufferSize = ((STREAMING_BUFFER_SIZE * bufferMultiplier) / blockAlign) * blockAlign;
    if (bufferSize == 0)
        bufferSize = blockAlign * 4096;  // Larger fallback buffer

    // Create multiple buffers for a proper ring buffer approach
    const int NUM_BUFFERS = 3;
    std::vector<std::vector<BYTE>> audioBuffers;
    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        audioBuffers.push_back(std::vector<BYTE>(bufferSize));
    }

    XAUDIO2_BUFFER xaudioBuffer = {};
    XAUDIO2_VOICE_STATE voiceState = {};

    // Initialize the ring buffer approach
    int currentBuffer = 0;
    int buffersInUse = 0;

    // FIX: Track which buffers have been submitted
    std::vector<bool> bufferSubmitted(NUM_BUFFERS, false);

    // Pre-load initial buffers
    {
        std::lock_guard<std::mutex> lock(streamingMutex);
        SetFilePointer(streamingFileHandle, dataChunkPosition, NULL, FILE_BEGIN);

        // Fill only the first buffer initially to prevent overlap/double playback
        // FIX: Load only one buffer instead of all NUM_BUFFERS
        if (totalBytesRead >= dataChunkSize)
        {
            // Loop back if needed
            totalBytesRead = 0;
            SetFilePointer(streamingFileHandle, dataChunkPosition, NULL, FILE_BEGIN);
        }

        // Calculate bytes to read, ensuring it's a multiple of blockAlign
        DWORD bytesToRead = static_cast<DWORD>(bufferSize);
        if (bytesToRead > (dataChunkSize - totalBytesRead))
            bytesToRead = static_cast<DWORD>((dataChunkSize - totalBytesRead) / blockAlign) * blockAlign;

        if (!ReadFile(streamingFileHandle, audioBuffers[0].data(), bytesToRead, &bytesRead, nullptr) || bytesRead == 0)
        {
            return; // Exit if read fails
        }

        // Adjust to ensure we have complete frames (avoid partial samples)
        DWORD alignedBytes = (bytesRead / blockAlign) * blockAlign;
        totalBytesRead += alignedBytes;

        // Submit this buffer
        xaudioBuffer = {};  // Reset the buffer
        xaudioBuffer.AudioBytes = alignedBytes;
        xaudioBuffer.pAudioData = audioBuffers[0].data();
        xaudioBuffer.Flags = 0;
        xaudioBuffer.pContext = reinterpret_cast<void*>(static_cast<uintptr_t>(0));  // Store buffer index as context

        if (FAILED(pSourceVoice->SubmitSourceBuffer(&xaudioBuffer)))
        {
            return; // Exit if submission fails
        }

        buffersInUse++;
        bufferSubmitted[0] = true;
    }

    // Create a HANDLE for an event that will notify us when a buffer finishes
    HANDLE bufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (bufferEndEvent == NULL)
    {
        std::cerr << "Failed to create buffer end event" << std::endl;
        return;
    }

    // Register for buffer end notifications
    XAUDIO2_VOICE_DETAILS voiceDetails;
    pSourceVoice->GetVoiceDetails(&voiceDetails);

    // Main streaming loop
    while (isStreaming)
    {
        // Get current buffer state
        pSourceVoice->GetState(&voiceState);

        // If we have room for another buffer
        if (voiceState.BuffersQueued < NUM_BUFFERS)
        {
            // Calculate which buffer to use next
            currentBuffer = (currentBuffer + 1) % NUM_BUFFERS;

            // Read the next chunk of data
            std::lock_guard<std::mutex> lock(streamingMutex);

            // Check if we need to loop back to beginning
            if (totalBytesRead >= dataChunkSize)
            {
                totalBytesRead = 0;
                SetFilePointer(streamingFileHandle, dataChunkPosition, NULL, FILE_BEGIN);
            }

            // Calculate bytes to read (ensure it's aligned to block size)
            DWORD bytesToRead = static_cast<DWORD>(bufferSize);
            if (bytesToRead > (dataChunkSize - totalBytesRead))
                bytesToRead = static_cast<DWORD>((dataChunkSize - totalBytesRead) / blockAlign) * blockAlign;

            if (!ReadFile(streamingFileHandle, audioBuffers[currentBuffer].data(), bytesToRead, &bytesRead, nullptr) || bytesRead == 0)
                break;

            // Ensure we only use complete frames
            DWORD alignedBytes = (bytesRead / blockAlign) * blockAlign;
            totalBytesRead += alignedBytes;

            // Submit buffer to XAudio2
            xaudioBuffer = {};  // Reset the buffer
            xaudioBuffer.AudioBytes = alignedBytes;
            xaudioBuffer.pAudioData = audioBuffers[currentBuffer].data();
            xaudioBuffer.Flags = 0;
            xaudioBuffer.pContext = reinterpret_cast<void*>(static_cast<uintptr_t>(currentBuffer));

            HRESULT hr = pSourceVoice->SubmitSourceBuffer(&xaudioBuffer);
            if (FAILED(hr))
            {
                std::cerr << "Failed to submit buffer: " << std::hex << hr << std::dec << std::endl;
                break;
            }

            buffersInUse++;
            bufferSubmitted[currentBuffer] = true;
        }
        else
        {
            // Wait a bit before checking again
            // Use a shorter wait time to be more responsive
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    // Clean up
    CloseHandle(bufferEndEvent);
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