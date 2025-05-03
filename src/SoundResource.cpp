/// <file>
/// <summary>
/// Sound Resource class for loading and managing sound files.
/// Encapsulates and models the sound resource for the audio engine.
/// </summary>
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-30</date>
/// </file>

#include "SoundResource.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include "PathUtils.h"

/// <summary>
/// Initializes the SoundResource object.
/// </summary>
SoundResource::SoundResource()
    : m_ResourceType(SOUND_EFFECT),
    m_IsStreaming(false),
    m_StreamingFileHandle(INVALID_HANDLE_VALUE),
    m_DataChunkSize(0),
    m_DataChunkPosition(0),
    m_ShouldLoop(false)
{
    ZeroMemory(&m_Buffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(&m_Wfx, sizeof(WAVEFORMATEXTENSIBLE));
}

/// <summary>
/// Cleans up the SoundResource object.
/// </summary>
SoundResource::~SoundResource()
{
    // Stop streaming if active
    StopStreaming();

    // Close file handle if open
    if (m_StreamingFileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_StreamingFileHandle);
        m_StreamingFileHandle = INVALID_HANDLE_VALUE;
    }
}

/// <summary>
/// Constructs a SoundResource and automatically loads the sound from the specified file path.
/// </summary>
/// <param name="filePath">Wide string path to the sound file</param>
/// <param name="type">Type of sound resource (SOUND_EFFECT or STREAMING)</param>
/// <param name="loop">Whether the sound should loop</param>
SoundResource::SoundResource(const std::wstring& filePath, ResourceType type, bool loop)
    : m_ResourceType(type),
    m_IsStreaming(false),
    m_StreamingFileHandle(INVALID_HANDLE_VALUE),
    m_DataChunkSize(0),
    m_DataChunkPosition(0),
    m_ShouldLoop(loop)
{
    ZeroMemory(&m_Buffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(&m_Wfx, sizeof(WAVEFORMATEXTENSIBLE));

    // Automatically load the sound
    Load(filePath, type, loop);
}

/// <summary>
/// Constructs a SoundResource and automatically loads the sound from the specified file path.
/// </summary>
/// <param name="filePath">String path to the sound file</param>
/// <param name="type">Type of sound resource (SOUND_EFFECT or STREAMING)</param>
/// <param name="loop">Whether the sound should loop</param>
SoundResource::SoundResource(const std::string& filePath, ResourceType type, bool loop)
    : m_ResourceType(type),
    m_IsStreaming(false),
    m_StreamingFileHandle(INVALID_HANDLE_VALUE),
    m_DataChunkSize(0),
    m_DataChunkPosition(0),
    m_ShouldLoop(loop)
{
    ZeroMemory(&m_Buffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(&m_Wfx, sizeof(WAVEFORMATEXTENSIBLE));

    // Convert string to wstring
    std::wstring wFilePath = PathUtils::StringToWString(filePath);

    // Automatically load the sound
    Load(wFilePath, type, loop);
}

/// <summary>
/// Loads a sound file from the specified path into the buffer.
/// </summary>
/// <param name="filePath">Path to the audio file</param>
/// <param name="type">Type of resource (SOUND_EFFECT or STREAMING)</param>
/// <param name="loop">Whether the sound should loop</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT SoundResource::Load(const std::wstring& filePath, ResourceType type, bool loop)
{
    m_FilePath = filePath;
    m_ResourceType = type;
    m_ShouldLoop = loop;

    // Ensure file path is absolute using PathUtils
    std::wstring absolutePath = PathUtils::GetAbsolutePath(filePath);

    // Log the path we're loading from
    wprintf(L"Loading sound from: %s\n", absolutePath.c_str());

    // Open file using wide-character API
    HANDLE hFile = CreateFileW(absolutePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD errorCode = GetLastError();
        wprintf(L"Failed to open file: %s, Error Code: %d\n", absolutePath.c_str(), errorCode);
        return HRESULT_FROM_WIN32(errorCode);
    }

    DWORD dwChunkSize = 0;
    DWORD dwChunkDataPosition = 0;

    // Find the 'RIFF' chunk
    HRESULT hr = FindChunk(hFile, FOURCC_RIFF, dwChunkSize, dwChunkDataPosition);
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

    hr = ReadChunkData(hFile, &m_Wfx, dwChunkSize, dwChunkDataPosition);
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
    m_DataChunkSize = dwChunkSize;
    m_DataChunkPosition = dwChunkDataPosition;

    if (m_ResourceType == SOUND_EFFECT)
    {
        // For sound effects, load the entire file into memory
        m_AudioData.resize(dwChunkSize);

        // Read the data into our vector
        hr = ReadChunkData(hFile, m_AudioData.data(), dwChunkSize, dwChunkDataPosition);
        if (FAILED(hr))
        {
            CloseHandle(hFile);
            return hr;
        }

        // Set up the audio buffer
        m_Buffer.AudioBytes = dwChunkSize;
        m_Buffer.pAudioData = m_AudioData.data();
        m_Buffer.Flags = XAUDIO2_END_OF_STREAM;

        // Set looping for sound effects if requested
        if (m_ShouldLoop)
        {
            m_Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
        }
        else
        {
            m_Buffer.LoopCount = 0;
        }

        CloseHandle(hFile);
    }
    else
    {
        // For streaming audio, keep the file handle open
        m_StreamingFileHandle = hFile;

        // We'll load data in chunks during playback
        m_Buffer.AudioBytes = 0;
        m_Buffer.pAudioData = nullptr;

        // Streaming audio uses manual looping in the worker thread
    }

    return S_OK;
}

/// <summary>
/// Play the sound resource.
/// </summary>
/// <param name="pXAudio2">XAudio2 engine instance</param>
/// <param name="ppSourceVoice">Pointer to source voice pointer</param>
/// <param name="volume">Sound volume</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT SoundResource::Play(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice, float volume)
{
    if (!pXAudio2 || !ppSourceVoice)
        return E_INVALIDARG;

    // Create a source voice if one wasn't provided
    if (*ppSourceVoice == nullptr)
    {
        HRESULT hr = pXAudio2->CreateSourceVoice(ppSourceVoice, (WAVEFORMATEX*)&m_Wfx);
        if (FAILED(hr))
            return hr;
    }

    // Set the volume
    HRESULT hr = SetVolume(*ppSourceVoice, volume);
    if (FAILED(hr))
        return hr;

    if (m_ResourceType == SOUND_EFFECT)
    {
        // For sound effects, just submit the buffer and play
        hr = (*ppSourceVoice)->SubmitSourceBuffer(&m_Buffer);
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
/// Set looping status
/// </summary>
/// <param name="shouldLoop">Whether the sound should loop</param>
void SoundResource::SetLooping(bool shouldLoop)
{
    m_ShouldLoop = shouldLoop;

    // If this is a sound effect and buffer is initialized, update the loop count
    if (m_ResourceType == SOUND_EFFECT && m_Buffer.pAudioData != nullptr)
    {
        m_Buffer.LoopCount = shouldLoop ? XAUDIO2_LOOP_INFINITE : 0;
    }
    // For streaming audio, the looping is handled in the streaming worker thread
}

/// <summary>
/// Stop the sound resource.
/// </summary>
/// <param name="pSourceVoice">Source voice to stop</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT SoundResource::Stop(IXAudio2SourceVoice* pSourceVoice)
{
    if (!pSourceVoice)
        return E_INVALIDARG;

    // Stop streaming if needed
    if (m_ResourceType == STREAMING)
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
/// <param name="pSourceVoice">Source voice</param>
/// <param name="volume">Volume level</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT SoundResource::SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume)
{
    if (!pSourceVoice)
        return E_INVALIDARG;

    // Clamp volume between 0.0 and 1.0
    volume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;

    return pSourceVoice->SetVolume(volume);
}

/// <summary>
/// Plays the streaming audio.
/// </summary>
/// <param name="pSourceVoice">Source voice</param>
/// <returns>HRESULT indicating success or failure</returns>
HRESULT SoundResource::StartStreaming(IXAudio2SourceVoice* pSourceVoice)
{
    if (m_StreamingFileHandle == INVALID_HANDLE_VALUE)
        return E_FAIL;

    // Stop any existing streaming
    StopStreaming();

    // Reset file position to the beginning of the data chunk
    SetFilePointer(m_StreamingFileHandle, m_DataChunkPosition, NULL, FILE_BEGIN);

    // Start streaming thread
    m_IsStreaming = true;
    m_StreamingThread = std::thread(&SoundResource::StreamingWorker, this, pSourceVoice);

    // Start playback
    return pSourceVoice->Start(0);
}

/// <summary>
/// Stops the streaming audio.
/// </summary>
void SoundResource::StopStreaming()
{
    if (m_IsStreaming)
    {
        m_IsStreaming = false;

        // Wait for streaming thread to finish
        if (m_StreamingThread.joinable())
        {
            m_StreamingThread.join();
        }
    }
}

/// <summary>
/// Worker function for streaming audio.
/// </summary>
/// <param name="pSourceVoice">Source voice</param>
void SoundResource::StreamingWorker(IXAudio2SourceVoice* pSourceVoice)
{
    if (!pSourceVoice)
        return;

    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;

    // Get the wave format block alignment to ensure proper buffer sizes
    const WAVEFORMATEX* pFormat = reinterpret_cast<const WAVEFORMATEX*>(&m_Wfx);
    DWORD blockAlign = pFormat->nBlockAlign;

    // Ensure buffer size is large enough and a multiple of the block alignment
    size_t bufferMultiplier = 4;
    size_t bufferSize = ((STREAMING_BUFFER_SIZE * bufferMultiplier) / blockAlign) * blockAlign;
    if (bufferSize == 0)
        bufferSize = blockAlign * 4096;

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

    // Track which buffers have been submitted
    std::vector<bool> bufferSubmitted(NUM_BUFFERS, false);

    // Pre-load initial buffers
    {
        std::lock_guard<std::mutex> lock(m_StreamingMutex);
        SetFilePointer(m_StreamingFileHandle, m_DataChunkPosition, NULL, FILE_BEGIN);

        // Fill only the first buffer initially to prevent overlap/double playback
        if (totalBytesRead >= m_DataChunkSize)
        {
            // Loop back if needed and if looping is enabled
            if (m_ShouldLoop)
            {
                totalBytesRead = 0;
                SetFilePointer(m_StreamingFileHandle, m_DataChunkPosition, NULL, FILE_BEGIN);
            }
            else
            {
                // End streaming if we're not looping
                return;
            }
        }

        // Calculate bytes to read, ensuring it's a multiple of blockAlign
        DWORD bytesToRead = static_cast<DWORD>(bufferSize);
        if (bytesToRead > (m_DataChunkSize - totalBytesRead))
            bytesToRead = static_cast<DWORD>((m_DataChunkSize - totalBytesRead) / blockAlign) * blockAlign;

        if (!ReadFile(m_StreamingFileHandle, audioBuffers[0].data(), bytesToRead, &bytesRead, nullptr) || bytesRead == 0)
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
    while (m_IsStreaming)
    {
        // Get current buffer state
        pSourceVoice->GetState(&voiceState);

        // If we have room for another buffer
        if (voiceState.BuffersQueued < NUM_BUFFERS)
        {
            // Calculate which buffer to use next
            currentBuffer = (currentBuffer + 1) % NUM_BUFFERS;

            // Read the next chunk of data
            std::lock_guard<std::mutex> lock(m_StreamingMutex);

            // Check if we need to loop back to beginning
            if (totalBytesRead >= m_DataChunkSize)
            {
                if (m_ShouldLoop)
                {
                    // Reset for looping
                    totalBytesRead = 0;
                    SetFilePointer(m_StreamingFileHandle, m_DataChunkPosition, NULL, FILE_BEGIN);
                }
                else
                {
                    // End of file and not looping - submit end buffer
                    xaudioBuffer = {};
                    xaudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
                    pSourceVoice->SubmitSourceBuffer(&xaudioBuffer);
                    break; // Exit the streaming loop
                }
            }

            // Calculate bytes to read (ensure it's aligned to block size)
            DWORD bytesToRead = static_cast<DWORD>(bufferSize);
            if (bytesToRead > (m_DataChunkSize - totalBytesRead))
                bytesToRead = static_cast<DWORD>((m_DataChunkSize - totalBytesRead) / blockAlign) * blockAlign;

            if (!ReadFile(m_StreamingFileHandle, audioBuffers[currentBuffer].data(), bytesToRead, &bytesRead, nullptr) || bytesRead == 0)
                break;

            // Ensure we only use complete frames
            DWORD alignedBytes = (bytesRead / blockAlign) * blockAlign;
            totalBytesRead += alignedBytes;

            // Submit buffer to XAudio2
            xaudioBuffer = {};  // Reset the buffer
            xaudioBuffer.AudioBytes = alignedBytes;
            xaudioBuffer.pAudioData = audioBuffers[currentBuffer].data();

            // Set end of stream flag if not looping and this is the last buffer
            if (!m_ShouldLoop && totalBytesRead >= m_DataChunkSize)
            {
                xaudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
            }
            else
            {
                xaudioBuffer.Flags = 0;
            }

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
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    // Clean up
    CloseHandle(bufferEndEvent);
}

/// <summary>
/// Finds a chunk in the file.
/// </summary>
/// <param name="hFile">File handle</param>
/// <param name="fourcc">Four character code to find</param>
/// <param name="dwChunkSize">Size of the chunk</param>
/// <param name="dwChunkDataPosition">Position of the chunk</param>
/// <returns>HRESULT indicating success or failure</returns>
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
        case FOURCC_RIFF:
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
/// <param name="hFile">File handle</param>
/// <param name="buffer">Buffer to read into</param>
/// <param name="buffersize">Size of the buffer</param>
/// <param name="bufferoffset">Offset in the file</param>
/// <returns>HRESULT indicating success or failure</returns>
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