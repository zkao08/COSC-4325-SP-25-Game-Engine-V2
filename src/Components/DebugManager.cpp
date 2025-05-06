// DebugManager.cpp
#include "DebugManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Initialize static instance
DebugManager* DebugManager::s_Instance = nullptr;

DebugManager::DebugManager()
    : m_Enabled(false),
    m_ConsoleHandle(NULL),
    m_FrameTime(0.0f),
    m_FPS(0.0f),
    m_FrameCount(0),
    m_TimeSinceLastFPSUpdate(0.0f),
    m_UpdateInterval(0.1f),     // Update console at 10Hz instead of every frame
    m_TimeSinceLastUpdate(0.0f),
    m_BufferWidth(120),
    m_BufferHeight(80),
    m_DebugStartLine(0)
{
}

DebugManager::~DebugManager()
{
    Shutdown();
}

DebugManager& DebugManager::GetInstance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = new DebugManager();
    }

    return *s_Instance;
}

bool DebugManager::Initialize()
{
    // Allocate a console if we don't have one
    if (AllocConsole())
    {
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cin.clear();
    }

    // Get console handle
    m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (m_ConsoleHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // Setup console properties
    SetupConsole();

    // Set enabled state
    m_Enabled = true;

    return true;
}

void DebugManager::SetupConsole()
{
    // Set console title
    SetConsoleTitleA("Game Engine Debug Console");

    // Configure console buffer size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_ConsoleHandle, &csbi);

    // Record current width for line clearing
    m_BufferWidth = csbi.dwSize.X;

    // Set larger buffer if needed
    COORD bufferSize = { m_BufferWidth, m_BufferHeight };
    SetConsoleScreenBufferSize(m_ConsoleHandle, bufferSize);

    // Disable cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(m_ConsoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(m_ConsoleHandle, &cursorInfo);

    // Disable console QuickEdit mode to prevent selection freezing updates
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(hInput, &prevMode);
    SetConsoleMode(hInput, prevMode & ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT));
}

void DebugManager::Update(float deltaTime, const InputHandler& inputHandler)
{
    if (!m_Enabled)
        return;

    // Update FPS counter
    m_FrameCount++;
    m_TimeSinceLastFPSUpdate += deltaTime;
    m_TimeSinceLastUpdate += deltaTime;
    m_FrameTime = deltaTime;

    // Update FPS at the regular interval
    if (m_TimeSinceLastFPSUpdate >= FPS_UPDATE_INTERVAL)
    {
        m_FPS = m_FrameCount / m_TimeSinceLastFPSUpdate;
        m_FrameCount = 0;
        m_TimeSinceLastFPSUpdate = 0.0f;
    }

    // Only update the console display at a reduced rate
    if (m_TimeSinceLastUpdate >= m_UpdateInterval)
    {
        // Update debug information display
        UpdateDebugInfo(inputHandler);

        // Reset timer
        m_TimeSinceLastUpdate = 0.0f;
    }
}

void DebugManager::UpdateDebugInfo(const InputHandler& inputHandler)
{
    // Build updated debug info
    std::vector<std::string> newDebugLines;

    // Performance info
    newDebugLines.push_back("===== PERFORMANCE INFO =====");
    std::stringstream fpsLine;
    fpsLine << "FPS: " << std::fixed << std::setprecision(1) << m_FPS;
    newDebugLines.push_back(fpsLine.str());

    std::stringstream frameLine;
    frameLine << "Frame Time: " << std::fixed << std::setprecision(2) << (m_FrameTime * 1000.0f) << " ms";
    newDebugLines.push_back(frameLine.str());
    newDebugLines.push_back("");

    // Keyboard info
    newDebugLines.push_back("===== KEYBOARD INFO =====");

    std::vector<int> pressedKeys;
    for (int key = 0; key <= 255; key++)
    {
        if (inputHandler.IsKeyDown(key))
        {
            pressedKeys.push_back(key);
        }
    }

    std::string keyLine = "Pressed keys: ";
    if (pressedKeys.empty())
    {
        keyLine += "None";
    }
    else
    {
        for (size_t i = 0; i < pressedKeys.size() && i < 10; i++)
        {
            int key = pressedKeys[i];
            keyLine += inputHandler.GetKeyName(key) + " ";
        }

        if (pressedKeys.size() > 10)
        {
            keyLine += "... (" + std::to_string(pressedKeys.size() - 10) + " more)";
        }
    }
    newDebugLines.push_back(keyLine);

    // Modifiers
    std::string modLine = "Modifiers: ";
    if (inputHandler.IsShiftDown()) modLine += "SHIFT ";
    if (inputHandler.IsCtrlDown()) modLine += "CTRL ";
    if (inputHandler.IsAltDown()) modLine += "ALT ";
    if (modLine == "Modifiers: ") modLine += "None";
    newDebugLines.push_back(modLine);
    newDebugLines.push_back("");

    // Mouse info
    newDebugLines.push_back("===== MOUSE INFO =====");

    const MouseState& mouse = inputHandler.GetMouseState();
    std::stringstream mousePos;
    mousePos << "Position: (" << mouse.x << ", " << mouse.y << ")";
    newDebugLines.push_back(mousePos.str());

    std::stringstream mouseDelta;
    mouseDelta << "Delta: (" << mouse.deltaX << ", " << mouse.deltaY << ")";
    newDebugLines.push_back(mouseDelta.str());

    std::stringstream mouseWheel;
    mouseWheel << "Wheel: " << mouse.wheelDelta;
    newDebugLines.push_back(mouseWheel.str());

    std::string buttonLine = "Buttons: ";
    if (mouse.buttons[MouseButton::LEFT]) buttonLine += "LEFT ";
    if (mouse.buttons[MouseButton::RIGHT]) buttonLine += "RIGHT ";
    if (mouse.buttons[MouseButton::MIDDLE]) buttonLine += "MIDDLE ";
    if (mouse.buttons[MouseButton::X1]) buttonLine += "X1 ";
    if (mouse.buttons[MouseButton::X2]) buttonLine += "X2 ";
    if (buttonLine == "Buttons: ") buttonLine += "None";
    newDebugLines.push_back(buttonLine);
    newDebugLines.push_back("");

    // Gamepad info (if available)
    if (inputHandler.IsGamepadAvailable())
    {
        const GamepadState& gamepad = inputHandler.GetGamepadState();

        newDebugLines.push_back("===== GAMEPAD INFO =====");
        newDebugLines.push_back("Type: " + std::string(inputHandler.GetGamepadName()));

        std::stringstream leftStick;
        leftStick << "Left Stick: ("
            << std::fixed << std::setprecision(2) << gamepad.leftStickX << ", "
            << std::fixed << std::setprecision(2) << gamepad.leftStickY << ")";
        newDebugLines.push_back(leftStick.str());

        std::stringstream rightStick;
        rightStick << "Right Stick: ("
            << std::fixed << std::setprecision(2) << gamepad.rightStickX << ", "
            << std::fixed << std::setprecision(2) << gamepad.rightStickY << ")";
        newDebugLines.push_back(rightStick.str());

        std::stringstream triggers;
        triggers << "Triggers: L=" << std::fixed << std::setprecision(2) << gamepad.leftTrigger
            << " R=" << std::fixed << std::setprecision(2) << gamepad.rightTrigger;
        newDebugLines.push_back(triggers.str());

        std::string buttonLine = "Buttons: ";
        if (gamepad.buttons[GamepadButton::A]) buttonLine += "A ";
        if (gamepad.buttons[GamepadButton::B]) buttonLine += "B ";
        if (gamepad.buttons[GamepadButton::X]) buttonLine += "X ";
        if (gamepad.buttons[GamepadButton::Y]) buttonLine += "Y ";
        if (gamepad.buttons[GamepadButton::LEFT_SHOULDER]) buttonLine += "LB ";
        if (gamepad.buttons[GamepadButton::RIGHT_SHOULDER]) buttonLine += "RB ";
        if (gamepad.buttons[GamepadButton::BACK]) buttonLine += "BACK ";
        if (gamepad.buttons[GamepadButton::START]) buttonLine += "START ";
        if (gamepad.buttons[GamepadButton::LEFT_THUMB]) buttonLine += "L3 ";
        if (gamepad.buttons[GamepadButton::RIGHT_THUMB]) buttonLine += "R3 ";
        if (gamepad.buttons[GamepadButton::DPAD_UP]) buttonLine += "UP ";
        if (gamepad.buttons[GamepadButton::DPAD_RIGHT]) buttonLine += "RIGHT ";
        if (gamepad.buttons[GamepadButton::DPAD_DOWN]) buttonLine += "DOWN ";
        if (gamepad.buttons[GamepadButton::DPAD_LEFT]) buttonLine += "LEFT ";
        if (buttonLine == "Buttons: ") buttonLine += "None";
        newDebugLines.push_back(buttonLine);
    }

    // Output the debug info to console
    if (m_DebugStartLine == 0)
    {
        // Get current cursor position after all startup text has been printed
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(m_ConsoleHandle, &csbi))
        {
            m_DebugStartLine = csbi.dwCursorPosition.Y;
        }
    }

    // The number of lines we need to handle (either erase or update)
    size_t maxLines = (std::max)(m_CurrentDebugLines.size(), newDebugLines.size());

    // Create a string of spaces to clear lines
    std::string spaces(m_BufferWidth - 1, ' ');

    for (size_t i = 0; i < maxLines; i++)
    {
        // Set cursor position for this line
        COORD position = { 0, m_DebugStartLine + (SHORT)i };
        SetConsoleCursorPosition(m_ConsoleHandle, position);

        // Clear this line
        std::cout << spaces;

        // Set cursor position back to start of line
        SetConsoleCursorPosition(m_ConsoleHandle, position);

        // If we have content for this line in the new debug info, write it
        if (i < newDebugLines.size())
        {
            std::cout << newDebugLines[i];
        }
    }

    // Store current debug lines for next update
    m_CurrentDebugLines = newDebugLines;
}

void DebugManager::LogStartupMessage(const std::string& message)
{
    if (!m_Enabled)
    {
        return;
    }

    // Log message
    std::cout << message << std::endl;

    // Reset debug start line so it will be recalculated after all startup messages
    m_DebugStartLine = 0;
}

void DebugManager::SetEnabled(bool enabled)
{
    m_Enabled = enabled;

    if (m_Enabled)
    {
        // Make console visible
        ShowWindow(GetConsoleWindow(), SW_SHOW);
    }
    else
    {
        // Hide console
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
}

bool DebugManager::IsEnabled() const
{
    return m_Enabled;
}

void DebugManager::Shutdown()
{
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }

    // Restore console input mode if needed
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    // Turn QuickEdit back on
    SetConsoleMode(hInput, mode | ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT);

    // Free console if needed
    // FreeConsole(); // Only free if we created it and don't need it anymore
}