// DebugManager.h
#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include "InputHandler.h"
#include "Timer.h"

class DebugManager
{
private:
    bool m_Enabled;
    HANDLE m_ConsoleHandle;

    // Performance metrics
    float m_FrameTime;
    float m_FPS;

    // FPS tracking
    int m_FrameCount;
    float m_TimeSinceLastFPSUpdate;
    const float FPS_UPDATE_INTERVAL = 0.5f; // Update FPS every half second

    // Update rate control
    float m_UpdateInterval;
    float m_TimeSinceLastUpdate;

    // Console buffer information
    int m_BufferWidth;
    int m_BufferHeight;
    int m_DebugStartLine;
    std::vector<std::string> m_CurrentDebugLines;

    // Helper methods for console manipulation
    void SetupConsole();
    void UpdateDebugInfo(const InputHandler& inputHandler);

    // Singleton implementation
    static DebugManager* s_Instance;
    DebugManager();

public:
    ~DebugManager();

    // Singleton access
    static DebugManager& GetInstance();

    // Initialize debug console
    bool Initialize();

    // Update debug information
    void Update(float deltaTime, const InputHandler& inputHandler);

    // Log startup messages
    void LogStartupMessage(const std::string& message);

    // Enable/disable debug display
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    // Clean up
    void Shutdown();
};