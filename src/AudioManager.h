#pragma once
#include <xaudio2.h>
#include <x3daudio.h>
#include <iostream>
#include <comdef.h>
#include <Windows.h>
#include <queue>
#include <string>
#include <unordered_map>

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

class AudioManager
{
	public:
		AudioManager();
		~AudioManager();

		bool startUp();
		void shutDown();

        // Plays a sound effect from a loaded buffer, optionally reusing the sound if it's frequently used
        HRESULT AudioManager::PlaySound(const std::string& filePath, bool reuse);

        void Update(); // To manually check for cleanup
        void DestroySourceVoice(IXAudio2SourceVoice* pSourceVoice);

    private:
        // Helper function to find a specific chunk in the audio file
        HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);

        // Helper function to read data from the chunk
        HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

        // Loads an audio file into the buffer
        HRESULT LoadAudioFile(const std::wstring& filePath, XAUDIO2_BUFFER& buffer, WAVEFORMATEXTENSIBLE& wfx);

        IXAudio2* pXAudio2;                     // Pointer to the XAudio2 engine
        IXAudio2MasteringVoice* pMasteringVoice; // Mastering voice for the audio output
        std::queue<IXAudio2SourceVoice*> sourceVoicePool; // Pool of available source voices
        const size_t MAX_POOL_SIZE = 10; // Max number of voices in the pool

        // Cached audio data for frequently used sounds
        std::unordered_map<std::wstring, std::pair<XAUDIO2_BUFFER, WAVEFORMATEXTENSIBLE>> m_cachedAudio;
};

//global singleton
extern AudioManager gAudioManager;

#endif