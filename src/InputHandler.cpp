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

    // Get character representation including shift state (for key naming)
    std::string GetCharWithShift(int keyCode, bool shifted)
    {
        // Handle special cases for punctuation and symbols
        switch (keyCode)
        {
        case VK_OEM_1:     return shifted ? ":" : ";";
        case VK_OEM_PLUS:  return shifted ? "+" : "=";
        case VK_OEM_COMMA: return shifted ? "<" : ",";
        case VK_OEM_MINUS: return shifted ? "_" : "-";
        case VK_OEM_PERIOD:return shifted ? ">" : ".";
        case VK_OEM_2:     return shifted ? "?" : "/";
        case VK_OEM_3:     return shifted ? "~" : "`";
        case VK_OEM_4:     return shifted ? "{" : "[";
        case VK_OEM_5:     return shifted ? "|" : "\\";
        case VK_OEM_6:     return shifted ? "}" : "]";
        case VK_OEM_7:     return shifted ? "\"" : "'";
        }

        // Handle number keys with shift
        if (keyCode >= '0' && keyCode <= '9')
        {
            if (shifted)
            {
                // Shift + number gives symbols
                switch (keyCode)
                {
                case '0': return ")";
                case '1': return "!";
                case '2': return "@";
                case '3': return "#";
                case '4': return "$";
                case '5': return "%";
                case '6': return "^";
                case '7': return "&";
                case '8': return "*";
                case '9': return "(";
                }
            }
            else
            {
                // Regular numbers
                return std::string(1, static_cast<char>(keyCode));
            }
        }

        // For letters, handle uppercase/lowercase
        if (keyCode >= 'A' && keyCode <= 'Z')
        {
            if (shifted)
                return std::string(1, static_cast<char>(keyCode)); // Uppercase
            else
                return std::string(1, static_cast<char>(keyCode + 32)); // Lowercase (ASCII shift)
        }

        // Default fallback
        return "";
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
    // Save previous key combo state
    m_PrevActiveKeyCombos = m_ActiveKeyCombos;
    m_ActiveKeyCombos.clear();

    // Save previous modifier state
    m_PreviousModifiers = m_CurrentModifiers;

    // Update keyboard state
    UpdateKeyboardState();

    // Update modifier state
    UpdateModifierState();

    // Update mouse state
    UpdateMouseState();

    // Update gamepad state
    UpdateGamepadState();

    // Update active key combos
    for (const auto& keyPair : m_KeyboardMap)
    {
        if (keyPair.second.down)
        {
            // For each down key, create combos with the current modifiers
            KeyCombo combo;
            combo.keyCode = keyPair.first;
            combo.modifiers = m_CurrentModifiers;
            m_ActiveKeyCombos.insert(combo);
        }
    }
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

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        int keyCode = static_cast<int>(wParam);
        auto it = m_KeyboardMap.find(keyCode);
        if (it != m_KeyboardMap.end())
        {
            it->second.downPrevious = it->second.down;
            it->second.down = true;
        }
        break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        int keyCode = static_cast<int>(wParam);
        auto it = m_KeyboardMap.find(keyCode);
        if (it != m_KeyboardMap.end())
        {
            it->second.downPrevious = it->second.down;
            it->second.down = false;
        }
        break;
    }

    case WM_LBUTTONDOWN:
        m_MouseState.buttonsPrev[MouseButton::LEFT] = m_MouseState.buttons[MouseButton::LEFT];
        m_MouseState.buttons[MouseButton::LEFT] = true;
        break;

    case WM_LBUTTONUP:
        m_MouseState.buttonsPrev[MouseButton::LEFT] = m_MouseState.buttons[MouseButton::LEFT];
        m_MouseState.buttons[MouseButton::LEFT] = false;
        break;

    case WM_RBUTTONDOWN:
        m_MouseState.buttonsPrev[MouseButton::RIGHT] = m_MouseState.buttons[MouseButton::RIGHT];
        m_MouseState.buttons[MouseButton::RIGHT] = true;
        break;

    case WM_RBUTTONUP:
        m_MouseState.buttonsPrev[MouseButton::RIGHT] = m_MouseState.buttons[MouseButton::RIGHT];
        m_MouseState.buttons[MouseButton::RIGHT] = false;
        break;

    case WM_MBUTTONDOWN:
        m_MouseState.buttonsPrev[MouseButton::MIDDLE] = m_MouseState.buttons[MouseButton::MIDDLE];
        m_MouseState.buttons[MouseButton::MIDDLE] = true;
        break;

    case WM_MBUTTONUP:
        m_MouseState.buttonsPrev[MouseButton::MIDDLE] = m_MouseState.buttons[MouseButton::MIDDLE];
        m_MouseState.buttons[MouseButton::MIDDLE] = false;
        break;

    case WM_XBUTTONDOWN:
    {
        int button = HIWORD(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
        m_MouseState.buttonsPrev[button] = m_MouseState.buttons[button];
        m_MouseState.buttons[button] = true;
        break;
    }

    case WM_XBUTTONUP:
    {
        int button = HIWORD(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
        m_MouseState.buttonsPrev[button] = m_MouseState.buttons[button];
        m_MouseState.buttons[button] = false;
        break;
    }

    // Add other input-related message handling if needed
    }
}

void InputHandler::InitializeKeyboardMap()
{
    // Initialize letters with both standard and shifted (uppercase) names
    for (int i = 'A'; i <= 'Z'; i++)
    {
        std::string keyName = std::string(1, static_cast<char>(i));
        std::string lowercaseName = std::string(1, static_cast<char>(i + 32));
        m_KeyboardMap[i] = KeyState{ false, false, keyName + " (" + lowercaseName + ")" };
    }

    // Initialize numbers with both standard and shifted symbol names
    for (int i = '0'; i <= '9'; i++)
    {
        std::string keyName = std::string(1, static_cast<char>(i));
        std::string shiftSymbol = GetCharWithShift(i, true);
        m_KeyboardMap[i] = KeyState{ false, false, keyName + " (" + shiftSymbol + ")" };
    }

    // Initialize function keys
    for (int i = VK_F1; i <= VK_F24; i++)
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
    m_KeyboardMap[VK_DELETE] = KeyState{ false, false, "DELETE" };
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

    // Initialize additional keys
    m_KeyboardMap[VK_OEM_1] = KeyState{ false, false, "; (:)" };
    m_KeyboardMap[VK_OEM_PLUS] = KeyState{ false, false, "= (+)" };
    m_KeyboardMap[VK_OEM_COMMA] = KeyState{ false, false, ", (<)" };
    m_KeyboardMap[VK_OEM_MINUS] = KeyState{ false, false, "- (_)" };
    m_KeyboardMap[VK_OEM_PERIOD] = KeyState{ false, false, ". (>)" };
    m_KeyboardMap[VK_OEM_2] = KeyState{ false, false, "/ (?)" };
    m_KeyboardMap[VK_OEM_3] = KeyState{ false, false, "` (~)" };
    m_KeyboardMap[VK_OEM_4] = KeyState{ false, false, "[ ({)" };
    m_KeyboardMap[VK_OEM_5] = KeyState{ false, false, "\\ (|)" };
    m_KeyboardMap[VK_OEM_6] = KeyState{ false, false, "] (})" };
    m_KeyboardMap[VK_OEM_7] = KeyState{ false, false, "' (\")" };

    // Numpad keys
    m_KeyboardMap[VK_NUMPAD0] = KeyState{ false, false, "NUMPAD 0" };
    m_KeyboardMap[VK_NUMPAD1] = KeyState{ false, false, "NUMPAD 1" };
    m_KeyboardMap[VK_NUMPAD2] = KeyState{ false, false, "NUMPAD 2" };
    m_KeyboardMap[VK_NUMPAD3] = KeyState{ false, false, "NUMPAD 3" };
    m_KeyboardMap[VK_NUMPAD4] = KeyState{ false, false, "NUMPAD 4" };
    m_KeyboardMap[VK_NUMPAD5] = KeyState{ false, false, "NUMPAD 5" };
    m_KeyboardMap[VK_NUMPAD6] = KeyState{ false, false, "NUMPAD 6" };
    m_KeyboardMap[VK_NUMPAD7] = KeyState{ false, false, "NUMPAD 7" };
    m_KeyboardMap[VK_NUMPAD8] = KeyState{ false, false, "NUMPAD 8" };
    m_KeyboardMap[VK_NUMPAD9] = KeyState{ false, false, "NUMPAD 9" };
    m_KeyboardMap[VK_MULTIPLY] = KeyState{ false, false, "NUMPAD *" };
    m_KeyboardMap[VK_ADD] = KeyState{ false, false, "NUMPAD +" };
    m_KeyboardMap[VK_SUBTRACT] = KeyState{ false, false, "NUMPAD -" };
    m_KeyboardMap[VK_DECIMAL] = KeyState{ false, false, "NUMPAD ." };
    m_KeyboardMap[VK_DIVIDE] = KeyState{ false, false, "NUMPAD /" };
    m_KeyboardMap[VK_SEPARATOR] = KeyState{ false, false, "NUMPAD SEPARATOR" };
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

void InputHandler::UpdateModifierState()
{
    // Start with no modifiers
    m_CurrentModifiers = 0;

    // Check each modifier key
    if (IsKeyDown(KeyCode::SHIFT_LEFT) || IsKeyDown(KeyCode::SHIFT_RIGHT))
        m_CurrentModifiers |= KeyCode::SHIFT_MASK;

    if (IsKeyDown(KeyCode::CONTROL_LEFT) || IsKeyDown(KeyCode::CONTROL_RIGHT))
        m_CurrentModifiers |= KeyCode::CTRL_MASK;

    if (IsKeyDown(KeyCode::ALT_LEFT) || IsKeyDown(KeyCode::ALT_RIGHT))
        m_CurrentModifiers |= KeyCode::ALT_MASK;

    if (IsKeyDown(KeyCode::SUPER_LEFT) || IsKeyDown(KeyCode::SUPER_RIGHT))
        m_CurrentModifiers |= KeyCode::WIN_MASK;
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

    // Update button states (if not already updated by event processing)
    if (!m_MouseState.buttons[MouseButton::LEFT])
        m_MouseState.buttons[MouseButton::LEFT] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    if (!m_MouseState.buttons[MouseButton::RIGHT])
        m_MouseState.buttons[MouseButton::RIGHT] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    if (!m_MouseState.buttons[MouseButton::MIDDLE])
        m_MouseState.buttons[MouseButton::MIDDLE] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    if (!m_MouseState.buttons[MouseButton::X1])
        m_MouseState.buttons[MouseButton::X1] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;

    if (!m_MouseState.buttons[MouseButton::X2])
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

bool InputHandler::IsModifierDown(int modifierMask) const
{
    return (m_CurrentModifiers & modifierMask) == modifierMask;
}

bool InputHandler::IsKeyComboDown(int keyCode, int modifierMask) const
{
    // First check if the key is down
    if (!IsKeyDown(keyCode))
        return false;

    // Then check if the modifiers match exactly
    return (m_CurrentModifiers == modifierMask);
}

bool InputHandler::IsKeyComboPressed(int keyCode, int modifierMask) const
{
    // Create the combo to look for
    KeyCombo currentCombo = { keyCode, modifierMask };

    // Check if it's in current active combos but not in previous active combos
    return (m_ActiveKeyCombos.find(currentCombo) != m_ActiveKeyCombos.end() &&
        m_PrevActiveKeyCombos.find(currentCombo) == m_PrevActiveKeyCombos.end());
}

bool InputHandler::IsKeyComboReleased(int keyCode, int modifierMask) const
{
    // Create the combo to look for
    KeyCombo currentCombo = { keyCode, modifierMask };

    // Check if it's not in current active combos but was in previous active combos
    return (m_ActiveKeyCombos.find(currentCombo) == m_ActiveKeyCombos.end() &&
        m_PrevActiveKeyCombos.find(currentCombo) != m_PrevActiveKeyCombos.end());
}

bool InputHandler::IsShiftKeyDown(int keyCode) const
{
    return IsKeyComboDown(keyCode, KeyCode::SHIFT_MASK);
}

bool InputHandler::IsShiftKeyPressed(int keyCode) const
{
    return IsKeyComboPressed(keyCode, KeyCode::SHIFT_MASK);
}

bool InputHandler::IsShiftKeyReleased(int keyCode) const
{
    return IsKeyComboReleased(keyCode, KeyCode::SHIFT_MASK);
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

bool InputHandler::IsShiftDown() const
{
    return IsModifierDown(KeyCode::SHIFT_MASK);
}

bool InputHandler::IsCtrlDown() const
{
    return IsModifierDown(KeyCode::CTRL_MASK);
}

bool InputHandler::IsAltDown() const
{
    return IsModifierDown(KeyCode::ALT_MASK);
}

int InputHandler::GetCurrentModifiers() const
{
    return m_CurrentModifiers;
}

void InputHandler::RegisterKeyCombo(const std::string& name, int keyCode, int modifiers)
{
    KeyCombo combo;
    combo.keyCode = keyCode;
    combo.modifiers = modifiers;
    m_RegisteredCombos[name] = combo;
}

bool InputHandler::IsComboDown(const std::string& name) const
{
    auto it = m_RegisteredCombos.find(name);
    if (it != m_RegisteredCombos.end())
    {
        const KeyCombo& combo = it->second;
        return IsKeyComboDown(combo.keyCode, combo.modifiers);
    }
    return false;
}

bool InputHandler::IsComboPressed(const std::string& name) const
{
    auto it = m_RegisteredCombos.find(name);
    if (it != m_RegisteredCombos.end())
    {
        const KeyCombo& combo = it->second;
        return IsKeyComboPressed(combo.keyCode, combo.modifiers);
    }
    return false;
}