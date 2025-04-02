// SoundResource.h
#pragma once
#include <xaudio2.h>
#include <string>
#include <vector>
#include <memory>

class AudioManager; // Forward declaration

class SoundResource {
public:
    SoundResource();
    ~SoundResource();

    // Load sound from file
    HRESULT Load(const std::wstring& filePath);
    
    // Play the sound
    HRESULT Play(IXAudio2* pXAudio2, IXAudio2SourceVoice** ppSourceVoice);
    
    // Accessors
    const XAUDIO2_BUFFER* GetBuffer() const { return &buffer; }
    const WAVEFORMATEXTENSIBLE* GetWaveFormat() const { return &wfx; }
    const std::wstring& GetFilePath() const { return filePath; }

private:
    std::vector<BYTE> audioData;       // Actual audio data, managed automatically
    XAUDIO2_BUFFER buffer;             // XAudio2 buffer structure
    WAVEFORMATEXTENSIBLE wfx;          // Wave format
    std::wstring filePath;             // Path to the audio file

    // Private helper methods
    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

    // Constants for WAV file format
    static const DWORD fourccRIFF = 'FFIR';
    static const DWORD fourccDATA = 'atad';
    static const DWORD fourccFMT = ' tmf';
    static const DWORD fourccWAVE = 'EVAW';
};