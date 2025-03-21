#include <iostream>

int main()
{
	//engine subsystems
	/**
	RenderManager renderManager;
	PhysicsManager physicsManager;
	etc..
	**/
	//AudioManager audioManager;
	
	//start up engine subsystems in correct order
	/**
	renderManager.startup();
	etc..
	**/
	
	//run the game engine
	std::cout << "Game Engine starting..." << std::endl;
	
	//shut down subsystems in reverse order
	/**
	etc..
	renderManager.shutdown();
	**/
	
	return 0;
}