#include <iostream>
#include "AudioManager.h"
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <sol/sol.hpp>
#include "AudioLuaAPI.h"

// Test Lua API integration
int TestLuaApi()
{
    // Audio manager is already initialized in main()
    std::cout << "\n=== Testing Audio Lua API ===\n" << std::endl;

    // Create a Lua state
    sol::state lua;

    // Open standard libraries
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::string, sol::lib::math);

    // Initialize the Audio Lua API
    AudioLuaAPI::Initialize(lua);

    // Load and run the test script
    try {
        lua.script_file("../assets/scripts/audio_lua_test.lua");
    }
    catch (const sol::error& e) {
        std::cerr << "Lua error: " << e.what() << std::endl;
    }

    std::cout << "\n=== Audio Lua API Test Complete ===\n" << std::endl;
    return 0;
}

// Test the audio memory management
void TestAudioMemoryManagement()
{
    std::cout << "\n=== Testing Audio Memory Management ===\n" << std::endl;

    // Configure the audio cache
    std::cout << "Configuring audio cache (max 10 resources, 2s min age)" << std::endl;
    gAudioManager.ConfigureCache(10, 2);

    // Create a list of test sound paths
    std::vector<std::string> testSounds;
    for (int i = 1; i <= 20; i++)
    {
        testSounds.push_back("assets/audio/sfx/test_sfx_" + std::to_string(i) + ".wav");
    }

    // Phase 1: Load many sounds
    std::cout << "\nPhase 1: Loading 15 different sounds" << std::endl;
    for (int i = 0; i < 15; i++)
    {
        std::string soundPath = testSounds[i];
        std::cout << "Loading sound: " << soundPath << std::endl;
        gAudioManager.PlaySound(soundPath, true, 0.1f);

        // Small delay to simulate game loop
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait longer to allow resources to age past the minimum
    std::cout << "\nWaiting for resources to age..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Manually trigger cleanup to configured maximum
    std::cout << "\nManually triggering cache cleanup to configured maximum:" << std::endl;
    gAudioManager.CleanupResourceCache();

    // Wait a short time
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Demonstrate override with aggressive cleanup
    std::cout << "\nDemonstrating aggressive cleanup to 5 resources:" << std::endl;
    gAudioManager.CleanupResourceCache(5);

    // Phase 2: Play some sounds repeatedly
    std::cout << "\nPhase 2: Playing a few sounds repeatedly (simulating frequent use)" << std::endl;
    for (int i = 0; i < 20; i++)
    {
        // Pick a sound from the first 5
        int soundIndex = i % 5;
        std::string soundPath = testSounds[soundIndex];
        std::cout << "Playing sound: " << soundPath << std::endl;
        gAudioManager.PlaySound(soundPath, true, 0.1f);

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // Wait to allow aging
    std::cout << "\nWaiting for resources to age..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Phase 3: Load more new sounds
    std::cout << "\nPhase 3: Loading more sounds (should keep frequently used ones)" << std::endl;
    for (int i = 5; i < 15; i++)
    {
        std::string soundPath = testSounds[i];
        std::cout << "Loading sound: " << soundPath << std::endl;
        gAudioManager.PlaySound(soundPath, true, 0.1f);

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait to allow aging
    std::cout << "\nWaiting for resources to age..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Final cleanup with different target
    std::cout << "\nFinal resource cleanup with override to 7 resources:" << std::endl;
    gAudioManager.CleanupResourceCache(7);

    std::cout << "\n=== Memory Management Test Complete ===\n" << std::endl;
}

int TestAudio()
{
    // 2. Set master volume to 70%
    std::cout << "Setting master volume to 70%" << std::endl;
    gAudioManager.SetMasterVolume(0.7f);

    // 3. Play background music with streaming (using 50% volume)
    std::string bgmPath = "assets\\audio\\bgm\\test_bgm.wav";
    std::cout << "Playing streaming BGM at 50% volume..." << std::endl;
    HRESULT hr = gAudioManager.PlaySound(bgmPath, false, 0.5f);  // false = streaming
    if (FAILED(hr)) {
        std::cerr << "Failed to play BGM." << std::endl;
        gAudioManager.shutDown();
        return -1;
    }

    // Give a moment for BGM to start playing
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 4. Play multiple sound effects concurrently with different volumes
    std::string sfxPath = "assets\\audio\\sfx\\test_sfx.wav";

    std::cout << "\nTesting concurrent sound playback with different volumes:" << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::cout << "Playing sound effect at 100% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 1.0f);

        // Play second sound after 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 60% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.6f);

        // Play third sound after another 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 30% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.3f);

        // Wait 3 seconds before next round
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // 5. Test changing volume of BGM while it's playing
    std::cout << "\nChanging BGM volume to 80%" << std::endl;
    gAudioManager.SetSoundVolume(bgmPath, 0.8f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing BGM volume to 20%" << std::endl;
    gAudioManager.SetSoundVolume(bgmPath, 0.2f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing BGM volume back to 50%" << std::endl;
    gAudioManager.SetSoundVolume(bgmPath, 0.5f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 6. Test changing master volume
    std::cout << "\nChanging master volume to 100%" << std::endl;
    gAudioManager.SetMasterVolume(1.0f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "\nTesting concurrent sound playback with 100% master volume:" << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::cout << "Playing sound effect at 100% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 1.0f);

        // Play second sound after 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 60% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.6f);

        // Play third sound after another 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 30% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.3f);

        // Wait 3 seconds before next round
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    std::cout << "Changing master volume to 30%" << std::endl;
    gAudioManager.SetMasterVolume(0.3f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "\nTesting concurrent sound playback with 30% master volume:" << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::cout << "Playing sound effect at 100% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 1.0f);

        // Play second sound after 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 60% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.6f);

        // Play third sound after another 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 30% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.3f);

        // Wait 3 seconds before next round
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    std::cout << "Changing master volume back to 70%" << std::endl;
    gAudioManager.SetMasterVolume(0.7f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "\nTesting concurrent sound playback with 70% master volume:" << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::cout << "Playing sound effect at 100% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 1.0f);

        // Play second sound after 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 60% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.6f);

        // Play third sound after another 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect at 30% volume" << std::endl;
        gAudioManager.PlaySound(sfxPath, true, 0.3f);

        // Wait 3 seconds before next round
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // 6.b. Test changing environment type
    std::cout << "\nChanging environment type to CAVE" << std::endl;
    gAudioManager.SetEnvironment(ENV_CAVE);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing environment type to UNDERWATER" << std::endl;
    gAudioManager.SetEnvironment(ENV_UNDERWATER);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing environment type to LARGE_HALL" << std::endl;
    gAudioManager.SetEnvironment(ENV_LARGE_HALL);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing environment type to NORMAL" << std::endl;
    gAudioManager.SetEnvironment(ENV_NORMAL);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 7. Test stopping specific sounds
    std::cout << "\nPlaying sound effect 1" << std::endl;
    gAudioManager.PlaySound(sfxPath, true, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Stopping sound effect 1" << std::endl;
    gAudioManager.StopSound(sfxPath);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 8. Test stopping BGM
    std::cout << "\nStopping BGM" << std::endl;
    gAudioManager.StopSound(bgmPath);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 9. Restart BGM
    std::cout << "Restarting BGM" << std::endl;
    gAudioManager.PlaySound(bgmPath, false, 0.7f);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 10. Stop all sounds and clean up
    std::cout << "\nStopping all sounds" << std::endl;
    gAudioManager.StopAllSounds();

    return 1;
}

int main()
{
    // 1. Initialize the audio manager
    if (!gAudioManager.startUp()) {
        std::cerr << "Audio Manager failed to start." << std::endl;
        return -1;
    }

    std::cout << "=== Audio Engine Testing ===\n" << std::endl;

    // Run standard audio tests
    //TestAudio();

    // Run memory management tests
    //TestAudioMemoryManagement();

    //Run Lua API tests
	TestLuaApi();

    std::cout << "\nShutting down AudioManager" << std::endl;
    gAudioManager.shutDown();

    std::cout << "\n=== Audio Engine Test Complete ===\n" << std::endl;
    return 0;
}