#include "GameEngine.h"
#include <iostream>
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
    GameEngine engine;
    
    if (!engine.Initialize())
    {
        std::cerr << "Failed to initialize game engine" << std::endl;
        return -1;
    }
    
    int result = engine.Run();
    
    return result;

}