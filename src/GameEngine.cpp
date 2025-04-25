// GameEngine.cpp
#include "GameEngine.h"
#include <Windows.h>
#include <iostream>

GameEngine::GameEngine() 
    : m_Running(false)
{
    // Empty constructor
}

GameEngine::~GameEngine()
{
    // Ensure proper shutdown
    if (m_Running)
    {
        Shutdown();
    }
}

bool GameEngine::Initialize()
{
    std::cout << "Initializing Game Engine..." << std::endl;
    
    try
    {
        // Initialize renderer (through Application)
        //m_Application = std::make_unique<Application>();
        
        // Initialize physics
        m_PhysicsSystem = std::make_unique<PhysicsWorld>(0.0f, -9.8f);
        
        // Initialize input
        m_InputSystem = std::make_unique<InputHandler>();
        
        // Initialize audio (singleton)
        if (!gAudioManager.startUp())
        {
            std::cerr << "Failed to initialize audio system" << std::endl;
            return false;
        }
        
        // Set running state
        m_Running = true;
        
        std::cout << "Game Engine initialized successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during engine initialization: " << e.what() << std::endl;
        Shutdown();
        return false;
    }
}

int GameEngine::Run()
{
    if (!m_Running)
    {
        std::cerr << "Error: Engine not initialized. Call Initialize() before Run()" << std::endl;
        return -1;
    }
    
    std::cout << "Starting game loop..." << std::endl;
    
    // Create and start timer
    Timer timer;
    timer.Start();
    
    // Main game loop
    while (m_Running)
    {
        // Calculate delta time
        timer.Tick();
        float deltaTime = timer.DeltaTime();
        
        // Process window messages
        ProcessWindowMessages();
        
        // Process input
        m_InputSystem->Update();
        
        // Update game logic
        Update(deltaTime);
        
        // Update physics
        m_PhysicsSystem->Step(deltaTime);
        
        // Render frame
        Render();
    }
    
    std::cout << "Game loop ended" << std::endl;
    return 0;
}

void GameEngine::Shutdown()
{
    std::cout << "Shutting down Game Engine..." << std::endl;
    
    // Set running flag to false
    m_Running = false;
    
    // Shutdown audio (singleton)
    gAudioManager.shutDown();
    
    // These will call destructors through reset()
    m_InputSystem.reset();
    m_PhysicsSystem.reset();
    //m_Application.reset();
    
    std::cout << "Game Engine shutdown complete" << std::endl;
}

void GameEngine::Update(float deltaTime)
{
    // Update game logic for game objects?
}

void GameEngine::Render()
{
    // These calls will be delegated to your Application and Renderer
    //m_Application->GetRenderer()->Clear();
    
    // Render objects would happen here
    // For example: for each object in game objects list, call render
    
    //m_Application->GetRenderer()->Present();
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