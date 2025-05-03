// SoundResource.h
#pragma once
#include <xaudio2.h>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class SoundResource
{
public:
    enum ResourceType
    {
        SOUND_EFFECT,  // Loaded entirely into memory
        STREAMING      // Streamed from disk in chunks
    };

    SoundResource();
    ~SoundResource();

    // Constructor that automatically loads a sound file
    SoundResource(const std::wstring& filePath, ResourceType type = SOUND_EFFECT, bool loop = false);

    // Constructor that takes string instead of wstring for convenience
    SoundResource(const std::string& filePath, ResourceType type = SOUND_EFFECT, bool loop = false);

    // Load sound from file
    HRESULT Load(const std::wstring& filePath, ResourceType type = SOUND_EFFECT, bool loop = false);

    // Play the sound with volume control (0.0f to 1.0f)
    HRESULT Play(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice, float volume = 1.0f);

    // Stop playback
    HRESULT Stop(IXAudio2SourceVoice* pSourceVoice);

    // Set volume for a given source voice (0.0f to 1.0f)
    HRESULT SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume);

    // Set looping status
    void SetLooping(bool shouldLoop);
    bool IsLooping() const { return m_ShouldLoop; }

    // Accessors
    const XAUDIO2_BUFFER* GetBuffer() const { return &m_Buffer; }
    const WAVEFORMATEXTENSIBLE* GetWaveFormat() const { return &m_Wfx; }
    const std::wstring& GetFilePath() const { return m_FilePath; }
    bool IsStreaming() const { return m_ResourceType == STREAMING; }

private:
    std::vector<BYTE> m_AudioData;       // Actual audio data, managed automatically
    XAUDIO2_BUFFER m_Buffer;             // XAudio2 buffer structure
    WAVEFORMATEXTENSIBLE m_Wfx;          // Wave format
    std::wstring m_FilePath;             // Path to the audio file
    ResourceType m_ResourceType;         // Type of sound resource
    bool m_ShouldLoop;                   // Whether the sound should loop

    // Streaming related members
    static const size_t STREAMING_BUFFER_SIZE = 65536;  // 64KB chunks for streaming
    std::atomic<bool> m_IsStreaming;
    std::thread m_StreamingThread;
    std::mutex m_StreamingMutex;
    HANDLE m_StreamingFileHandle;

    // Start the streaming process for BGM
    HRESULT StartStreaming(IXAudio2SourceVoice* pSourceVoice);

    // Stop the streaming process
    void StopStreaming();

    // Streaming worker function
    void StreamingWorker(IXAudio2SourceVoice* pSourceVoice);

    // Private helper methods
    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

    // Store wave file header locations for streaming
    DWORD m_DataChunkSize;
    DWORD m_DataChunkPosition;

    // Constants for WAV file format
    static const DWORD fourccRIFF = 'FFIR';
    static const DWORD fourccDATA = 'atad';
    static const DWORD fourccFMT = ' tmf';
    static const DWORD fourccWAVE = 'EVAW';
};