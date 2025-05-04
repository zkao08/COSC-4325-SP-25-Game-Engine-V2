/// <file>
/// <summary>
/// Lua API for the AudioManager and SoundResource classes.
/// Exposes only necessary functions for game developers.
/// </summary>
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-30</date>
/// </file>

#include "AudioLuaAPI.h"
#include <iostream>

namespace AudioLuaAPI
{
    /// <summary>
    /// Initializes the Lua API for the AudioManager and SoundResource classes.
    /// </summary>
    /// <param name="lua"></param>
    void Initialize(sol::state& lua)
    {
        // Create table for the audio API
        sol::table audioAPI = lua.create_named_table("Audio");

        // Expose enums
        // ResourceType enum
        audioAPI.new_enum("ResourceType",
            "SOUND_EFFECT", SoundResource::SOUND_EFFECT,
            "STREAMING", SoundResource::STREAMING
        );

        // EnvironmentType enum
        audioAPI.new_enum("EnvironmentType",
            "NORMAL", ENV_NORMAL,
            "CAVE", ENV_CAVE,
            "UNDERWATER", ENV_UNDERWATER,
            "LARGE_HALL", ENV_LARGE_HALL
        );

        // Register SoundResource class
        lua.new_usertype<SoundResource>("SoundResource",
            // Constructor
            sol::constructors<
            SoundResource(const std::string&, SoundResource::ResourceType),
            SoundResource(const std::string&, SoundResource::ResourceType, bool)
            > (),

            // Methods
            "GetFilePath", &SoundResource::GetFilePath,
            "IsStreaming", &SoundResource::IsStreaming,
            "IsLooping", &SoundResource::IsLooping,
            "SetLooping", &SoundResource::SetLooping
        );

        // Get manager instances
        AudioManager& audioManager = AudioManager::GetInstance();
        ResourceManager& resourceManager = ResourceManager::GetInstance();

        // Register sound management functions
        audioAPI.set_function("LoadSound", sol::overload(
            [&resourceManager](const std::string& id, const std::string& filePath, bool isStreaming, bool loop) -> std::shared_ptr<SoundResource> {
                return resourceManager.LoadSound(id, filePath, isStreaming, loop);
            },
            [&resourceManager](const std::string& id, const std::string& filePath, bool isStreaming) -> std::shared_ptr<SoundResource> {
                return resourceManager.LoadSound(id, filePath, isStreaming, false);
            },
            [&resourceManager](const std::string& id, const std::string& filePath) -> std::shared_ptr<SoundResource> {
                return resourceManager.LoadSound(id, filePath, false, false);
            }
        ));

        // Play sound + overloads
        audioAPI.set_function("PlaySound", sol::overload(
            [&audioManager](const std::string& soundId, float volume) -> bool {
                HRESULT hr = audioManager.PlaySound(soundId, volume);
                return SUCCEEDED(hr);
            },
            [&audioManager](const std::string& soundId) -> bool {
                HRESULT hr = audioManager.PlaySound(soundId, 1.0f);
                return SUCCEEDED(hr);
            },
            [&audioManager, &resourceManager](const std::string& filePath, bool isStreaming, float volume, bool loop) -> bool {
                // Create a unique ID for the sound based on the filepath
                std::string id = "sound_" + filePath;

                // Load the sound first
                resourceManager.LoadSound(id, filePath, isStreaming, loop);

                // Then play it
                HRESULT hr = audioManager.PlaySound(id, volume);
                return SUCCEEDED(hr);
            },
            [&audioManager](SoundResource* pSoundResource, float volume) -> bool {
                HRESULT hr = audioManager.PlaySoundDirect(pSoundResource, volume);
                return SUCCEEDED(hr);
            },
            [&audioManager](SoundResource* pSoundResource) -> bool {
                HRESULT hr = audioManager.PlaySoundDirect(pSoundResource, 1.0f);
                return SUCCEEDED(hr);
            }
        ));

        // Stop sound functions
        audioAPI.set_function("StopSound", [&audioManager](const std::string& soundId) -> bool {
            HRESULT hr = audioManager.StopSound(soundId);
            return SUCCEEDED(hr);
            });

        audioAPI.set_function("StopAllSounds", [&audioManager]() {
            audioManager.StopAllSounds();
            });

        // Volume functions
        audioAPI.set_function("SetSoundVolume", [&audioManager](const std::string& soundId, float volume) -> bool {
            HRESULT hr = audioManager.SetSoundVolume(soundId, volume);
            return SUCCEEDED(hr);
            });

        audioAPI.set_function("SetMasterVolume", [&audioManager](float volume) -> bool {
            HRESULT hr = audioManager.SetMasterVolume(volume);
            return SUCCEEDED(hr);
            });

        // Environment / reverb settings
        audioAPI.set_function("SetEnvironment", [&audioManager](EnvironmentType envType) -> bool {
            HRESULT hr = audioManager.SetEnvironment(envType);
            return SUCCEEDED(hr);
            });

        // Memory management functions
        audioAPI.set_function("UnloadSound", [&resourceManager](const std::string& id) {
            resourceManager.UnloadSound(id);
            });

        audioAPI.set_function("UnloadUnusedSounds", sol::overload(
            [&resourceManager]() {
                resourceManager.UnloadUnusedSounds();
            },
            [&resourceManager](int maxAgeInSeconds) {
                resourceManager.UnloadUnusedSounds(maxAgeInSeconds);
            }
        ));

        audioAPI.set_function("ConfigureSoundCache", sol::overload(
            [&resourceManager]() {
                resourceManager.ConfigureSoundCache(50, 10);
            },
            [&resourceManager](size_t maxSounds) {
                resourceManager.ConfigureSoundCache(maxSounds, 10);
            },
            [&resourceManager](size_t maxSounds, size_t minAgeSeconds) {
                resourceManager.ConfigureSoundCache(maxSounds, minAgeSeconds);
            }
        ));

        // Voice pool management
        audioAPI.set_function("ConfigureSourceVoicePool", [&audioManager](size_t maxVoices) {
            audioManager.ConfigureSourceVoicePool(maxVoices);
            });

        audioAPI.set_function("IsVoicePlaying", [&audioManager](const std::string& soundId) -> bool {
            return audioManager.IsVoicePlaying(soundId);
            });

        // GetVoicePoolStatus returns a table with the voice pool statistics
        audioAPI.set_function("GetVoicePoolStatus", [&audioManager](sol::this_state ts) -> sol::table {
            sol::state_view lua(ts);
            size_t totalVoices = 0;
            size_t playingVoices = 0;

            audioManager.GetVoicePoolStatus(totalVoices, playingVoices);

            sol::table result = lua.create_table();
            result["totalVoices"] = totalVoices;
            result["playingVoices"] = playingVoices;

            return result;
            });

        audioAPI.set_function("PerformMaintenance", [&audioManager]() {
            audioManager.PerformMaintenance();
            });

        std::cout << "Audio Lua API initialized successfully" << std::endl;
    }
}