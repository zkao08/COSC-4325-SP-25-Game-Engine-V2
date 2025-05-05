#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include "Application.h"

int main(int argc, char* argv[])
{
	GameEngine engine;
	engine.Initialize(true);
	engine.Run(true); //run in debug mode
	engine.Shutdown();
}