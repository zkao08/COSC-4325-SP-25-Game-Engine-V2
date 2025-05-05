// main.cpp
#include "GameEngine.h"

// Standard main function entry point
int main(int argc, char* argv[])
{
	GameEngine engine;
	engine.Initialize(false); //run in runtime mode
	engine.Run(false); //run without debug mode
	engine.Shutdown();
}