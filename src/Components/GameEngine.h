// Updated GameEngine.h
#pragma once

#include "Application.h"
#include "PhysicsWorld.h"
#include "InputHandler.h"
#include "AudioManager.h"
#include "ResourceManager.h"
#include "Timer.h"
#include "DebugManager.h"
#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

class Application;
class Object;

class GameEngine
{
private:
    // Core subsystems in .cpp file

    // Applications
    static Application* gApplication;
    static std::unique_ptr<Application> gRuntime;

    // Engine state
    bool m_Running;
    bool m_DebugMode;

    // Process window messages
    void ProcessWindowMessages();

    // Resource management
    const float RESOURCE_MANAGEMENT_INTERVAL = 10.0f; // seconds

public:
    GameEngine();
    ~GameEngine();

    // Core lifecycle
    bool Initialize();
    int Run(bool debug = false);
    void Shutdown();

    // Main update and render methods
    void Update(float deltaTime);

    // Runtime
    static void GameEngine::CreateRuntime(std::string title = "Runtime", Object* gameObject = nullptr);
    static void GameEngine::DestroyRuntime();


    // Debug controls
    void EnableDebug(bool enable);
    bool IsDebugEnabled() const;
};