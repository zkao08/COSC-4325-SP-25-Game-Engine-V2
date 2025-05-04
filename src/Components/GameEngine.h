// GameEngine.h
#pragma once

#include "Object.h"
#include <memory>
#include <string>

class GameEngine 
{
private:
    // Core subsystems in .cpp file

    // Application pointers
    static Application* gApplication;
    static Application* gRuntime;
    
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

    // Runtime
    static void CreateRuntime(std::string title = "Runtime", Object* game_object = nullptr);
    
    // Main update and render methods
    void Update(float deltaTime);
};