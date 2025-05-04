// InputHandler.cpp
// Implementation of the InputHandler class

#include "InputHandler.h"
#include <algorithm>

// Helper functions
namespace
{
    // Deadzone for analog sticks
    constexpr float GAMEPAD_DEADZONE = 0.15f;

    // Apply deadzone to analog input
    float ApplyDeadzone(float value, float deadzone)
    {
        if (value < -deadzone)
        {
            return (value + deadzone) / (1.0f - deadzone);
        }
        else if (value > deadzone)
        {
            return (value - deadzone) / (1.0f - deadzone);
        }
        return 0.0f;
    }

    // Convert button index to XInput button mask
    WORD GamepadButtonToXInput(int button)
    {
        switch (button)
        {
        case GamepadButton::A:             return XINPUT_GAMEPAD_A;
        case GamepadButton::B:             return XINPUT_GAMEPAD_B;
        case GamepadButton::X:             return XINPUT_GAMEPAD_X;
        case GamepadButton::Y:             return XINPUT_GAMEPAD_Y;
        case GamepadButton::LEFT_SHOULDER: return XINPUT_GAMEPAD_LEFT_SHOULDER;
        case GamepadButton::RIGHT_SHOULDER: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
        case GamepadButton::BACK:          return XINPUT_GAMEPAD_BACK;
        case GamepadButton::START:         return XINPUT_GAMEPAD_START;
        case GamepadButton::LEFT_THUMB:    return XINPUT_GAMEPAD_LEFT_THUMB;
        case GamepadButton::RIGHT_THUMB:   return XINPUT_GAMEPAD_RIGHT_THUMB;
        case GamepadButton::DPAD_UP:       return XINPUT_GAMEPAD_DPAD_UP;
        case GamepadButton::DPAD_RIGHT:    return XINPUT_GAMEPAD_DPAD_RIGHT;
        case GamepadButton::DPAD_DOWN:     return XINPUT_GAMEPAD_DPAD_DOWN;
        case GamepadButton::DPAD_LEFT:     return XINPUT_GAMEPAD_DPAD_LEFT;
        default:                           return 0;
        }
    }
}

InputHandler::InputHandler() : m_Hwnd(nullptr)
{
    // Initialize keyboard map with key names
    InitializeKeyboardMap();
}

InputHandler::~InputHandler()
{
    // Nothing to clean up
}

void InputHandler::Initialize(HWND hwnd)
{
    m_Hwnd = hwnd;
}

void InputHandler::Update()
{
    // Update keyboard state
    UpdateKeyboardState();

    // Update mouse state
    UpdateMouseState();

    // Update gamepad state
    UpdateGamepadState();
}

void InputHandler::ProcessMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        m_MouseState.wheelDelta = static_cast<float>(zDelta);
        break;
    }

    // Add other input-related message handling if needed
    }
}

void InputHandler::InitializeKeyboardMap()
{
    // Initialize letters
    for (int i = 'A'; i <= 'Z'; i++)
    {
        char keyName[2] = { static_cast<char>(i), '\0' };
        m_KeyboardMap[i] = KeyState{ false, false, keyName };
    }

    // Initialize numbers
    for (int i = '0'; i <= '9'; i++)
    {
        char keyName[2] = { static_cast<char>(i), '\0' };
        m_KeyboardMap[i] = KeyState{ false, false, keyName };
    }

    // Initialize function keys
    for (int i = VK_F1; i <= VK_F12; i++)
    {
        m_KeyboardMap[i] = KeyState{ false, false, "F" + std::to_string(i - VK_F1 + 1) };
    }

    // Initialize special keys
    m_KeyboardMap[VK_ESCAPE] = KeyState{ false, false, "ESC" };
    m_KeyboardMap[VK_SPACE] = KeyState{ false, false, "SPACE" };
    m_KeyboardMap[VK_RETURN] = KeyState{ false, false, "ENTER" };
    m_KeyboardMap[VK_BACK] = KeyState{ false, false, "BACKSPACE" };
    m_KeyboardMap[VK_TAB] = KeyState{ false, false, "TAB" };
    m_KeyboardMap[VK_CAPITAL] = KeyState{ false, false, "CAPS LOCK" };
    m_KeyboardMap[VK_LSHIFT] = KeyState{ false, false, "L SHIFT" };
    m_KeyboardMap[VK_RSHIFT] = KeyState{ false, false, "R SHIFT" };
    m_KeyboardMap[VK_LCONTROL] = KeyState{ false, false, "L CTRL" };
    m_KeyboardMap[VK_RCONTROL] = KeyState{ false, false, "R CTRL" };
    m_KeyboardMap[VK_LMENU] = KeyState{ false, false, "L ALT" };
    m_KeyboardMap[VK_RMENU] = KeyState{ false, false, "R ALT" };
    m_KeyboardMap[VK_LWIN] = KeyState{ false, false, "L WIN" };
    m_KeyboardMap[VK_RWIN] = KeyState{ false, false, "R WIN" };
    m_KeyboardMap[VK_APPS] = KeyState{ false, false, "MENU" };
    m_KeyboardMap[VK_INSERT] = KeyState{ false, false, "INSERT" };
    m_KeyboardMap[VK_DELETE] = KeyState{ false, false, "DELETE_KEY" };
    m_KeyboardMap[VK_HOME] = KeyState{ false, false, "HOME" };
    m_KeyboardMap[VK_END] = KeyState{ false, false, "END" };
    m_KeyboardMap[VK_PRIOR] = KeyState{ false, false, "PAGE UP" };
    m_KeyboardMap[VK_NEXT] = KeyState{ false, false, "PAGE DOWN" };
    m_KeyboardMap[VK_UP] = KeyState{ false, false, "UP" };
    m_KeyboardMap[VK_DOWN] = KeyState{ false, false, "DOWN" };
    m_KeyboardMap[VK_LEFT] = KeyState{ false, false, "LEFT" };
    m_KeyboardMap[VK_RIGHT] = KeyState{ false, false, "RIGHT" };
    m_KeyboardMap[VK_SNAPSHOT] = KeyState{ false, false, "PRINT SCREEN" };
    m_KeyboardMap[VK_SCROLL] = KeyState{ false, false, "SCROLL LOCK" };
    m_KeyboardMap[VK_PAUSE] = KeyState{ false, false, "PAUSE" };
    m_KeyboardMap[VK_NUMLOCK] = KeyState{ false, false, "NUM LOCK" };
}

void InputHandler::UpdateKeyboardState()
{
    // First, update all previous key states
    for (auto& keyPair : m_KeyboardMap)
    {
        keyPair.second.downPrevious = keyPair.second.down;
    }

    // Update current key states
    for (auto& keyPair : m_KeyboardMap)
    {
        int keyCode = keyPair.first;
        keyPair.second.down = (GetAsyncKeyState(keyCode) & 0x8000) != 0;
    }
}

void InputHandler::UpdateMouseState()
{
    // Update previous mouse position and buttons
    m_MouseState.prevX = m_MouseState.x;
    m_MouseState.prevY = m_MouseState.y;
    for (int i = 0; i < 5; i++)
    {
        m_MouseState.buttonsPrev[i] = m_MouseState.buttons[i];
    }

    // Get current mouse position
    POINT p;
    if (GetCursorPos(&p) && m_Hwnd != NULL)
    {
        ScreenToClient(m_Hwnd, &p);
        m_MouseState.x = p.x;
        m_MouseState.y = p.y;
    }

    // Calculate delta
    m_MouseState.deltaX = m_MouseState.x - m_MouseState.prevX;
    m_MouseState.deltaY = m_MouseState.y - m_MouseState.prevY;

    // Update button states
    m_MouseState.buttons[MouseButton::LEFT] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    m_MouseState.buttons[MouseButton::RIGHT] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    m_MouseState.buttons[MouseButton::MIDDLE] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    m_MouseState.buttons[MouseButton::X1] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
    m_MouseState.buttons[MouseButton::X2] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
}

void InputHandler::UpdateGamepadState()
{
    // Update each gamepad
    for (int i = 0; i < 4; i++)
    {
        // Save previous button states
        for (int j = 0; j < 14; j++)
        {
            m_GamepadStates[i].buttonsPrev[j] = m_GamepadStates[i].buttons[j];
        }

        // Get new state
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD result = XInputGetState(i, &state);
        m_GamepadStates[i].connected = (result == ERROR_SUCCESS);

        if (m_GamepadStates[i].connected)
        {
            // Update button states
            for (int j = 0; j < 14; j++)
            {
                WORD buttonMask = GamepadButtonToXInput(j);
                m_GamepadStates[i].buttons[j] = (state.Gamepad.wButtons & buttonMask) != 0;
            }

            // Update analog sticks with deadzone
            float rawLeftX = state.Gamepad.sThumbLX / 32767.0f;
            float rawLeftY = state.Gamepad.sThumbLY / 32767.0f;
            float rawRightX = state.Gamepad.sThumbRX / 32767.0f;
            float rawRightY = state.Gamepad.sThumbRY / 32767.0f;

            m_GamepadStates[i].leftStickX = ApplyDeadzone(rawLeftX, GAMEPAD_DEADZONE);
            m_GamepadStates[i].leftStickY = ApplyDeadzone(rawLeftY, GAMEPAD_DEADZONE);
            m_GamepadStates[i].rightStickX = ApplyDeadzone(rawRightX, GAMEPAD_DEADZONE);
            m_GamepadStates[i].rightStickY = ApplyDeadzone(rawRightY, GAMEPAD_DEADZONE);

            // Update triggers
            m_GamepadStates[i].leftTrigger = state.Gamepad.bLeftTrigger / 255.0f;
            m_GamepadStates[i].rightTrigger = state.Gamepad.bRightTrigger / 255.0f;
        }
        else
        {
            // Reset states if disconnected
            for (int j = 0; j < 14; j++)
            {
                m_GamepadStates[i].buttons[j] = false;
            }

            m_GamepadStates[i].leftStickX = 0.0f;
            m_GamepadStates[i].leftStickY = 0.0f;
            m_GamepadStates[i].rightStickX = 0.0f;
            m_GamepadStates[i].rightStickY = 0.0f;
            m_GamepadStates[i].leftTrigger = 0.0f;
            m_GamepadStates[i].rightTrigger = 0.0f;
        }
    }
}

bool InputHandler::IsKeyDown(int keyCode) const
{
    auto it = m_KeyboardMap.find(keyCode);
    if (it != m_KeyboardMap.end())
    {
        return it->second.down;
    }
    return false;
}

bool InputHandler::IsKeyPressed(int keyCode) const
{
    auto it = m_KeyboardMap.find(keyCode);
    if (it != m_KeyboardMap.end())
    {
        return it->second.down && !it->second.downPrevious;
    }
    return false;
}

bool InputHandler::IsKeyReleased(int keyCode) const
{
    auto it = m_KeyboardMap.find(keyCode);
    if (it != m_KeyboardMap.end())
    {
        return !it->second.down && it->second.downPrevious;
    }
    return false;
}

const std::string& InputHandler::GetKeyName(int keyCode) const
{
    auto it = m_KeyboardMap.find(keyCode);
    if (it != m_KeyboardMap.end())
    {
        return it->second.name;
    }
    return m_UnknownKey;
}

bool InputHandler::IsMouseButtonDown(int button) const
{
    if (button >= 0 && button < 5)
    {
        return m_MouseState.buttons[button];
    }
    return false;
}

bool InputHandler::IsMouseButtonPressed(int button) const
{
    if (button >= 0 && button < 5)
    {
        return m_MouseState.buttons[button] && !m_MouseState.buttonsPrev[button];
    }
    return false;
}

bool InputHandler::IsMouseButtonReleased(int button) const
{
    if (button >= 0 && button < 5)
    {
        return !m_MouseState.buttons[button] && m_MouseState.buttonsPrev[button];
    }
    return false;
}

int InputHandler::GetMouseX() const
{
    return m_MouseState.x;
}

int InputHandler::GetMouseY() const
{
    return m_MouseState.y;
}

int InputHandler::GetMouseDeltaX() const
{
    return m_MouseState.deltaX;
}

int InputHandler::GetMouseDeltaY() const
{
    return m_MouseState.deltaY;
}

float InputHandler::GetMouseWheelDelta() const
{
    return m_MouseState.wheelDelta;
}

bool InputHandler::IsGamepadAvailable(int gamepadIndex) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4)
    {
        return m_GamepadStates[gamepadIndex].connected;
    }
    return false;
}

bool InputHandler::IsGamepadButtonDown(int gamepadIndex, int button) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4 && button >= 0 && button < 14)
    {
        return m_GamepadStates[gamepadIndex].buttons[button];
    }
    return false;
}

bool InputHandler::IsGamepadButtonPressed(int gamepadIndex, int button) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4 && button >= 0 && button < 14)
    {
        return m_GamepadStates[gamepadIndex].buttons[button] &&
            !m_GamepadStates[gamepadIndex].buttonsPrev[button];
    }
    return false;
}

bool InputHandler::IsGamepadButtonReleased(int gamepadIndex, int button) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4 && button >= 0 && button < 14)
    {
        return !m_GamepadStates[gamepadIndex].buttons[button] &&
            m_GamepadStates[gamepadIndex].buttonsPrev[button];
    }
    return false;
}

float InputHandler::GetGamepadAxisValue(int gamepadIndex, int axis) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4)
    {
        const auto& pad = m_GamepadStates[gamepadIndex];
        switch (axis)
        {
        case GamepadAxis::LEFT_X:
            return pad.leftStickX;
        case GamepadAxis::LEFT_Y:
            return pad.leftStickY;
        case GamepadAxis::RIGHT_X:
            return pad.rightStickX;
        case GamepadAxis::RIGHT_Y:
            return pad.rightStickY;
        case GamepadAxis::LEFT_TRIGGER:
            return pad.leftTrigger;
        case GamepadAxis::RIGHT_TRIGGER:
            return pad.rightTrigger;
        default:
            return 0.0f;
        }
    }
    return 0.0f;
}

const char* InputHandler::GetGamepadName(int gamepadIndex) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4 && m_GamepadStates[gamepadIndex].connected)
    {
        return "Xbox Controller"; // XInput doesn't provide a way to get the controller name
    }
    return "No Gamepad";
}

const MouseState& InputHandler::GetMouseState() const
{
    return m_MouseState;
}

const GamepadState& InputHandler::GetGamepadState(int gamepadIndex) const
{
    if (gamepadIndex >= 0 && gamepadIndex < 4)
    {
        return m_GamepadStates[gamepadIndex];
    }

    // Return the first gamepad state as fallback
    return m_GamepadStates[0];
}