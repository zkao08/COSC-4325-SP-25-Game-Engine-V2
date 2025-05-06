#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include "Application.h"

// Launch the game engine in editor mode
int main(int argc, char* argv[])
{
	GameEngine engine;
	engine.Initialize(true); //run in editor mode
	engine.Run(true); //run in debug mode
	engine.Shutdown();
}