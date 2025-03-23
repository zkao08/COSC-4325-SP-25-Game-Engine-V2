#include <iostream>
#include "AudioManager.h"
#include <thread>
#include <chrono>

int main()
{
	/* pseudocode
	
	//start up engine subsystems in correct order
	if (!gAudioManager.startUp())
	{
		std::cerr << "Audio system failed to initialize. Exiting..." << std::endl;
		return -1;
	}

	//run the game engine
	std::cout << "Game Engine starting..." << std::endl;
	
	//shut down subsystems in reverse order
	gAudioManager.shutDown();
	
	return 0;
	
	*/

	/* testing audio subsytem */
    // 1. Initialize the audio manager
    if (!gAudioManager.startUp()) {
        std::cerr << "Audio Manager failed to start." << std::endl;
        return -1;
    }

    // 2. Play a longer sound (BGM)
    std::string bgmPath = "assets\\audio\\bgm\\test_bgm.wav";
    HRESULT hr = gAudioManager.PlaySound(bgmPath, false);
    if (FAILED(hr)) {
        std::cerr << "Failed to play BGM." << std::endl;
        gAudioManager.shutDown();
        return -1;
    }
    std::cout << "BGM is playing..." << std::endl;

    // 3. Play a sound effect repeatedly at intervals
    std::string soundEffectPath = "assets\\audio\\sfx\\test_sfx.wav";
    for (int i = 0; i < 5; ++i) {
        hr = gAudioManager.PlaySound(soundEffectPath, true);
        if (FAILED(hr)) {
            std::cerr << "Failed to play sound effect." << std::endl;
            gAudioManager.shutDown();
            return -1;
        }

        // Wait for 2 seconds before playing the next sound effect
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Step 4: Wait some time before shutting down
    std::this_thread::sleep_for(std::chrono::seconds(30));  // Simulate some wait time for background music

    // Step 5: Shut down the audio manager
    std::cout << "Shutting down AudioManager after playing sound effects." << std::endl;
    gAudioManager.shutDown();

    return 0;
}