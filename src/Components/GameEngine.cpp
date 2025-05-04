// Updated relevant portions of GameEngine.cpp
#include "GameEngine.h"
#include "Application.h"
#include "PhysicsWorld.h"
#include "InputHandler.h"
#include "AudioManager.h"
#include "ResourceManager.h"
#include "Timer.h"
#include "DebugManager.h"
#include <Windows.h>
#include <iostream>

GameEngine::GameEngine()
    : m_Running(false),
    m_DebugMode(false)
{
    // Empty constructor
}

GameEngine::~GameEngine()
{
	// Destructor
}

bool GameEngine::Initialize()
{
    std::cout << "Initializing Game Engine..." << std::endl;

    try
    {
        // Get singleton references
        PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
        InputHandler& gInputSystem = InputHandler::GetInstance();
        AudioManager& gAudioManager = AudioManager::GetInstance();
        ResourceManager& gResourceManager = ResourceManager::GetInstance();
        DebugManager& gDebugManager = DebugManager::GetInstance();

        // Initialize debug manager early so we can use it for logging
        if (!gDebugManager.Initialize())
        {
            std::cerr << "Warning: Failed to initialize debug manager" << std::endl;
        }

        // Initialize subsystems
        // Initialize app and renderer
        //TODO: replace with actual renderer initialization
        //gDebugManager.LogStartupMessage("Initializing renderer...");

        // Initialize physics
        gDebugManager.LogStartupMessage("Initializing physics system...");
        if (!gPhysicsSystem.startUp(0.0f, -9.8f))
        {
            std::cerr << "Failed to initialize physics system" << std::endl;
            gDebugManager.LogStartupMessage("Physics system initialization failed!");
            return false;
        }
        gDebugManager.LogStartupMessage("Physics system initialized successfully");

        // Initialize input
        gDebugManager.LogStartupMessage("Initializing input system...");
        HWND hwnd = GetConsoleWindow(); // TODO: replace with actual window handle
        if (!gInputSystem.startUp(hwnd))
        {
            std::cerr << "Failed to initialize input system" << std::endl;
            gDebugManager.LogStartupMessage("Input system initialization failed!");
            return false;
        }
        gDebugManager.LogStartupMessage("Input system initialized successfully");

        // Initialize audio
        gDebugManager.LogStartupMessage("Initializing audio manager...");
        if (!gAudioManager.startUp())
        {
            std::cerr << "Failed to initialize audio system" << std::endl;
            gDebugManager.LogStartupMessage("AudioManager initialization failed!");
            return false;
        }
        gDebugManager.LogStartupMessage("AudioManager initialized successfully");

        // Initialize resource manager
        gDebugManager.LogStartupMessage("Initializing resource manager...");
        if (!gResourceManager.startUp(nullptr)) // TODO: pass actual renderer
        {
            std::cerr << "Failed to initialize resource manager system" << std::endl;
            gDebugManager.LogStartupMessage("Resource manager initialization failed!");
            return false;
        }
        gDebugManager.LogStartupMessage("Resource manager initialized successfully");

        // Set running state
        m_Running = true;

        gDebugManager.LogStartupMessage("Game Engine initialized successfully");
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during engine initialization: " << e.what() << std::endl;
        Shutdown();
        return false;
    }
}

int GameEngine::Run(bool debug)
{
    if (!m_Running)
    {
        std::cerr << "Error: Engine not initialized. Call Initialize() before Run()" << std::endl;
        return -1;
    }

    // Set debug mode
    EnableDebug(debug);

    // Get singleton references
    PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
    InputHandler& gInputSystem = InputHandler::GetInstance();
    AudioManager& gAudioManager = AudioManager::GetInstance();
    ResourceManager& gResourceManager = ResourceManager::GetInstance();
    DebugManager& gDebugManager = DebugManager::GetInstance();

    std::cout << "Starting game loop..." << std::endl;

    // Create and start timer
    Timer timer;
    timer.Start();
    float resourceManagementTimer = 0.0f;

    // Main game loop
    while (m_Running)
    {
        // Calculate delta time
        timer.Tick();
        float deltaTime = timer.DeltaTime();
        resourceManagementTimer += deltaTime;

        // Process window messages
        ProcessWindowMessages();

        // Process input
        gInputSystem.Update();

        // Update game logic
        Update(deltaTime);

        // Update physics
        gPhysicsSystem.Step(deltaTime);

        // Render frame
        //gRenderer.Render();

        // Run resource management
        if (resourceManagementTimer >= RESOURCE_MANAGEMENT_INTERVAL)
        {
            gResourceManager.PerformMaintenance();
            resourceManagementTimer = 0.0f;
        }

        // Update debug display if enabled
        if (m_DebugMode)
        {
            gDebugManager.Update(deltaTime, gInputSystem);
        }

        // Sleep to limit frame rate 
        //TODO: replace with actual frame rate governance
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    std::cout << "Game loop ended" << std::endl;
    return 0;
}

void GameEngine::Shutdown()
{
    std::cout << "Shutting down Game Engine..." << std::endl;

    // Get singleton references
    PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
    InputHandler& gInputSystem = InputHandler::GetInstance();
    AudioManager& gAudioManager = AudioManager::GetInstance();
    ResourceManager& gResourceManager = ResourceManager::GetInstance();
    DebugManager& gDebugManager = DebugManager::GetInstance();

    // Set running flag to false
    m_Running = false;

    // Shutdown debug manager first
    if (m_DebugMode)
    {
        gDebugManager.Shutdown();
    }

    // Shutdown other subsystems
    gAudioManager.shutDown();
    gInputSystem.shutDown();
    gPhysicsSystem.shutDown();
    gResourceManager.shutDown();

    std::cout << "Game Engine shutdown complete" << std::endl;
}

void GameEngine::Update(float deltaTime)
{
    // Update game logic for game objects
}

void GameEngine::EnableDebug(bool enable)
{
    m_DebugMode = enable;

    // Access debug manager and enable/disable it
    DebugManager& debugManager = DebugManager::GetInstance();

    if (m_DebugMode)
    {
        // Make sure debug manager is initialized
        if (!debugManager.IsEnabled())
        {
            debugManager.Initialize();
        }

        debugManager.SetEnabled(true);
    }
    else
    {
        debugManager.SetEnabled(false);
    }
}

bool GameEngine::IsDebugEnabled() const
{
    return m_DebugMode;
}

void GameEngine::ProcessWindowMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            m_Running = false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}