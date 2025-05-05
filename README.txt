COSC-4325 Game Engine
===================

Introduction
-----------

This repository contains a 2D game engine developed as part of the COSC-4325 Spring 2025 course. 
The engine features a renderer using DirectX 11, a physics system based on Box2D, 
an audio system using XAudio2, and an input handling system using Windows API and XInput. 
The project includes both a runtime environment for playing games and a level editor for creating game content.

Getting Started
-------------

Test executables along with the game engine runtime and level editor executables can be found in 
COSC-4325-SP-25-Game-Engine-V2/demo.

Building the Engine:

To build the game engine and the test executables, run the following commands in the root directory:

    cmake -B build
    cmake --build build

The first command creates a build directory and generates the build files. 
The second command compiles the source code and produces the executable files.

Executables and Instructions
--------------------------

AudioTest.exe - Runs a suite of audio-related tests.
- No input necessary, the test suite runs automatically and plays audio on its own
- Tests include playback of sound effects, background music, volume control, and environmental audio effects

GameEngine.exe - Runs the runtime version of the game engine.
- A runtime game window should open
- No default level is loaded

InputTest.exe - Runs a command line-based test for input processing and DirectX integration.
- WARNING: Console may flash during operation
- Detects and displays mouse, keyboard, and gamepad input in the console
- Shows real-time input state changes

PhysicsTest.exe - Runs a physics simulation test.
- Use arrow keys left/right to apply horizontal force to objects
- Use arrow keys up/down to set gravity direction (up or down)
- Demonstrates various physics shapes and properties

LevelEditor.exe - Runs the level editor version of the game engine.
- Opens the level editor interface with dockable windows
- A debug console will display performance metrics and input processing
- Create game objects, set properties, arrange scenes with the on screen buttons
- Middle mouse button to drag objects in the inspector
- Middle mouse button to move the camera within the viewport