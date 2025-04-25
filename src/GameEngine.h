// GameEngine.h
#pragma once

#include "Application.h"
#include "PhysicsWorld.h"
#include "InputHandler.h"
#include "AudioManager.h"
#include "Timer.h"

#include <memory>
#include <string>

class GameEngine 
{
private:
    // Core subsystems
    //std::unique_ptr<Application> m_Application;
    std::unique_ptr<PhysicsWorld> m_PhysicsSystem;
    std::unique_ptr<InputHandler> m_InputSystem;
    
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
    void Render();
};