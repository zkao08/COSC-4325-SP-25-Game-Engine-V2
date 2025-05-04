// main.cpp
#include "GameEngine.h"

// Standard main function entry point
int main(int argc, char* argv[])
{
	GameEngine engine;
	engine.Initialize();
	engine.Run();
	engine.Shutdown();
}