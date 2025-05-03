// GameEngine.h
#pragma once

#include <memory>
#include <string>

class GameEngine 
{
private:
    // Core subsystems in .cpp file
    
    // Engine state
    bool m_Running;
    
    // Process window messages
    void ProcessWindowMessages();

public:
    GameEngine();
    ~GameEngine();
    
    // Core lifecycle
    bool Initialize();
    int Run();
    void Shutdown();
    
    // Main update and render methods
    void Update(float deltaTime);
};