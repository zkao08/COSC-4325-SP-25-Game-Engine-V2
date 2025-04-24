#include <iostream>
#include "AudioManager.h"
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <sol/sol.hpp>
#include "AudioLuaAPI.h"
#include "Application.h"

int main()
{
    int status = 0;

    // 1. Initialize the audio manager
    if (!gAudioManager.startUp()) {
        std::cerr << "Audio Manager failed to start." << std::endl;
        return -1;
    }

    // Initialize the GUI and renderer
    std::unique_ptr<Application> app = std::make_unique<Application>();
    status = app->Execute();

    std::cout << "\nShutting down AudioManager" << std::endl;
    gAudioManager.shutDown();

    return status;
}