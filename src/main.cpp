// main.cpp
#include "GameEngine.h"

// Standard main function entry point
int main(int argc, char* argv[])
{
	GameEngine engine;
	engine.Initialize(false);
	engine.Run(false); //run in debug mode
	engine.Shutdown();
}