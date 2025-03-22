#include <iostream>
#include "AudioManager.h"

int main()
{
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
}