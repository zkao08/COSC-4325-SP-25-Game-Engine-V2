// Updated GameEngine.h
#pragma once

#include <memory>
#include <string>
#include <thread>

class GameEngine
{
private:
    // Core subsystems in .cpp file

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

    // Debug controls
    void EnableDebug(bool enable);
    bool IsDebugEnabled() const;
};