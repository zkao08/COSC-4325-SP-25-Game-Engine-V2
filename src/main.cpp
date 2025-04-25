#include "GameEngine.h"
#include <iostream>
<<<<<<< Updated upstream
#include "AudioManager.h"
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <sol/sol.hpp>
#include "AudioLuaAPI.h"

int main()
{
    // 1. Initialize the audio manager
    if (!gAudioManager.startUp()) {
        std::cerr << "Audio Manager failed to start." << std::endl;
        return -1;
    }

    std::cout << "\nShutting down AudioManager" << std::endl;
    gAudioManager.shutDown();
    return 0;
=======

int main()
{
    GameEngine engine;
    
    if (!engine.Initialize())
    {
        std::cerr << "Failed to initialize game engine" << std::endl;
        return -1;
    }
    
    int result = engine.Run();
    
    return result;
>>>>>>> Stashed changes
}