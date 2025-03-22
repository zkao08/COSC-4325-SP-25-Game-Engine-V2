/// <file>
/// <summary>
/// Audio Manager using XAudio2 and X3DAudio
/// for the audio engine subsystem of the game engine
/// </summary>
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-3-21 </date>

#include "AudioManager.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <iostream>
#include <comdef.h>

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
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM: " << _com_error(hr).ErrorMessage() << std::endl;
        return false;
    }

    hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        std::cerr << "Failed to create XAudio2 engine: " << _com_error(hr).ErrorMessage() << std::endl;
        CoUninitialize(); // Clean up COM if initialization failed
        return false;
    }

    hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr)) {
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
    if (pMasteringVoice) {
        pMasteringVoice->DestroyVoice();
        pMasteringVoice = nullptr;
    }

    if (pXAudio2) {
        pXAudio2->Release();
        pXAudio2 = nullptr;
    }

    CoUninitialize();
    std::cout << "AudioManager shut down." << std::endl;
}