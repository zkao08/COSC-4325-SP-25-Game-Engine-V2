#include <iostream>
#include "AudioManager.h"
#include <thread>
#include <chrono>
#include <string>

int main()
{
    // 1. Initialize the audio manager
    if (!gAudioManager.startUp()) {
        std::cerr << "Audio Manager failed to start." << std::endl;
        return -1;
    }

    std::cout << "=== Audio Engine Testing ===\n" << std::endl;

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
    std::string sfx1Path = "assets\\audio\\sfx\\test_sfx.wav";

    std::cout << "\nTesting concurrent sound playback with different volumes:" << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::cout << "Playing sound effect 1 at 100% volume" << std::endl;
        gAudioManager.PlaySound(sfx1Path, true, 1.0f);

        // Play second sound after 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect 2 at 60% volume" << std::endl;
        gAudioManager.PlaySound(sfx1Path, true, 0.6f);

        // Play third sound after another 0.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Playing sound effect 3 at 30% volume" << std::endl;
        gAudioManager.PlaySound(sfx1Path, true, 0.3f);

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

    std::cout << "Changing master volume to 30%" << std::endl;
    gAudioManager.SetMasterVolume(0.3f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Changing master volume back to 70%" << std::endl;
    gAudioManager.SetMasterVolume(0.7f);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 7. Test stopping specific sounds
    std::cout << "\nPlaying sound effect 1" << std::endl;
    gAudioManager.PlaySound(sfx1Path, true, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Stopping sound effect 1" << std::endl;
    gAudioManager.StopSound(sfx1Path);
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

    std::cout << "\nShutting down AudioManager" << std::endl;
    gAudioManager.shutDown();

    std::cout << "\n=== Audio Engine Test Complete ===\n" << std::endl;
    return 0;
}
