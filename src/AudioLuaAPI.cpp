/// <file>
/// <summary>
/// Lua API for the AudioManager and SoundResource classes.
/// Exposes only necessary functions for game developers.
/// </summary>
/// 
/// <author> Zachary Kao </author>
/// <date> 2025-4-19</date>
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
                SoundResource(const std::string&, SoundResource::ResourceType)
            > (),

            // Methods
            "GetFilePath", &SoundResource::GetFilePath,
            "IsStreaming", &SoundResource::IsStreaming
        );

        // Get AudioManager instance
        AudioManager& audioManager = AudioManager::GetInstance();

        // Register AudioManager functions

        // Play sound + overloads
        audioAPI.set_function("PlaySound", sol::overload(
            [&audioManager](const std::string& filePath, bool isSoundEffect, float volume) -> bool {
                HRESULT hr = audioManager.PlaySound(filePath, isSoundEffect, volume);
                return SUCCEEDED(hr);
            },
            [&audioManager](const std::string& filePath, bool isSoundEffect) -> bool {
                HRESULT hr = audioManager.PlaySound(filePath, isSoundEffect);
                return SUCCEEDED(hr);
            },
            [&audioManager](const std::string& filePath) -> bool {
                HRESULT hr = audioManager.PlaySound(filePath);
                return SUCCEEDED(hr);
            },
            [&audioManager](SoundResource* pSoundResource, float volume) -> bool {
                HRESULT hr = audioManager.PlaySound(pSoundResource, volume);
                return SUCCEEDED(hr);
            },
            [&audioManager](SoundResource* pSoundResource) -> bool {
                HRESULT hr = audioManager.PlaySound(pSoundResource);
                return SUCCEEDED(hr);
            }
        ));

		// Stop sound functions
        audioAPI.set_function("StopSound", [&audioManager](const std::string& filePath) -> bool {
            HRESULT hr = audioManager.StopSound(filePath);
            return SUCCEEDED(hr);
            });

        audioAPI.set_function("StopAllSounds", [&audioManager]() {
            audioManager.StopAllSounds();
            });

        // Volume functions
        audioAPI.set_function("SetSoundVolume", [&audioManager](const std::string& filePath, float volume) -> bool {
            HRESULT hr = audioManager.SetSoundVolume(filePath, volume);
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
        audioAPI.set_function("CleanupResourceCache", sol::overload(
            [&audioManager]() {
                audioManager.CleanupResourceCache();
            },
            [&audioManager](size_t maxResourcesOverride) {
                audioManager.CleanupResourceCache(maxResourcesOverride);
            }
        ));

        audioAPI.set_function("ConfigureCache", sol::overload(
            [&audioManager]() {
                audioManager.ConfigureCache();
            },
            [&audioManager](size_t maxCachedResources) {
                audioManager.ConfigureCache(maxCachedResources);
            },
            [&audioManager](size_t maxCachedResources, size_t minResourceAge) {
                audioManager.ConfigureCache(maxCachedResources, minResourceAge);
            },
            [&audioManager](size_t maxCachedResources, size_t minResourceAge, size_t maxSourceVoices) {
                audioManager.ConfigureCache(maxCachedResources, minResourceAge, maxSourceVoices);
            }
        ));

        std::cout << "Audio Lua API initialized successfully" << std::endl;
    }
}