// GameEngine.cpp
#include "GameEngine.h"
#include "Application.h"
#include "PhysicsWorld.h"
#include "InputHandler.h"
#include "AudioManager.h"
#include "ResourceManager.h"
#include "Timer.h"
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
        // Get singleton references
        //Renderer& gRenderer = Renderer::GetInstance();
        //PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
        //InputHandler& gInputSystem = InputSystem::GetInstance();
        AudioManager& gAudioManager = AudioManager::GetInstance();
        //ResourceManager& gResourceManager = ResourceManager::GetInstance();
        
        // Initialize subsystems
        //gPhysicsSystem.startUp(0.0f, -9.8f);
        //gInputSystem.startUp();
        
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

    // Get singleton references
    //Renderer& gRenderer = Renderer::GetInstance();
    //PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
    //InputHandler& gInputSystem = InputSystem::GetInstance();
    AudioManager& gAudioManager = AudioManager::GetInstance();
    //ResourceManager& gResourceManager = ResourceManager::GetInstance();
    
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
        //gInputSystem->Update();
        
        // Update game logic
        Update(deltaTime);
        
        // Update physics
        //gPhysicsSystem->Step(deltaTime);
        
        // Render frame
        //gRenderer.Render();

        //TODO: run resource management every once in a while
    }
    
    std::cout << "Game loop ended" << std::endl;
    return 0;
}

void GameEngine::Shutdown()
{
    std::cout << "Shutting down Game Engine..." << std::endl;
    
    // Get singleton references
    //Renderer& gRenderer = Renderer::GetInstance();
    //PhysicsWorld& gPhysicsSystem = PhysicsWorld::GetInstance();
    //InputHandler& gInputSystem = InputSystem::GetInstance();
    AudioManager& gAudioManager = AudioManager::GetInstance();
    //ResourceManager& gResourceManager = ResourceManager::GetInstance();

    // Set running flag to false
    m_Running = false;
    
    // Shutdown subsystems
    gAudioManager.shutDown();
    //gInputSystem.shutDown();
    //gPhysicsSystem.shutDown();
    //m_Application.shutDown();
    
    std::cout << "Game Engine shutdown complete" << std::endl;
}

void GameEngine::Update(float deltaTime)
{
    // Update game logic for game objects
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