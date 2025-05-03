// InputHandler.h
// Cross-platform input handling using Windows API and XInput with modifier key support

#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <unordered_set>

// Required library for XInput
#pragma comment(lib, "Xinput.lib")

// Key code constants (compatible with virtual key codes)
namespace KeyCode
{
    // Letters
    constexpr int A = 'A';
    constexpr int B = 'B';
    constexpr int C = 'C';
    constexpr int D = 'D';
    constexpr int E = 'E';
    constexpr int F = 'F';
    constexpr int G = 'G';
    constexpr int H = 'H';
    constexpr int I = 'I';
    constexpr int J = 'J';
    constexpr int K = 'K';
    constexpr int L = 'L';
    constexpr int M = 'M';
    constexpr int N = 'N';
    constexpr int O = 'O';
    constexpr int P = 'P';
    constexpr int Q = 'Q';
    constexpr int R = 'R';
    constexpr int S = 'S';
    constexpr int T = 'T';
    constexpr int U = 'U';
    constexpr int V = 'V';
    constexpr int W = 'W';
    constexpr int X = 'X';
    constexpr int Y = 'Y';
    constexpr int Z = 'Z';

    // Numbers
    constexpr int NUM_0 = '0';
    constexpr int NUM_1 = '1';
    constexpr int NUM_2 = '2';
    constexpr int NUM_3 = '3';
    constexpr int NUM_4 = '4';
    constexpr int NUM_5 = '5';
    constexpr int NUM_6 = '6';
    constexpr int NUM_7 = '7';
    constexpr int NUM_8 = '8';
    constexpr int NUM_9 = '9';

    // Function keys
    constexpr int F1 = VK_F1;
    constexpr int F2 = VK_F2;
    constexpr int F3 = VK_F3;
    constexpr int F4 = VK_F4;
    constexpr int F5 = VK_F5;
    constexpr int F6 = VK_F6;
    constexpr int F7 = VK_F7;
    constexpr int F8 = VK_F8;
    constexpr int F9 = VK_F9;
    constexpr int F10 = VK_F10;
    constexpr int F11 = VK_F11;
    constexpr int F12 = VK_F12;

    // Special keys
    constexpr int ESCAPE = VK_ESCAPE;
    constexpr int SPACE = VK_SPACE;
    constexpr int ENTER = VK_RETURN;
    constexpr int BACKSPACE = VK_BACK;
    constexpr int TAB = VK_TAB;
    constexpr int CAPS_LOCK = VK_CAPITAL;
    constexpr int SHIFT_LEFT = VK_LSHIFT;
    constexpr int SHIFT_RIGHT = VK_RSHIFT;
    constexpr int CONTROL_LEFT = VK_LCONTROL;
    constexpr int CONTROL_RIGHT = VK_RCONTROL;
    constexpr int ALT_LEFT = VK_LMENU;
    constexpr int ALT_RIGHT = VK_RMENU;
    constexpr int SUPER_LEFT = VK_LWIN;
    constexpr int SUPER_RIGHT = VK_RWIN;
    constexpr int MENU = VK_APPS;
    constexpr int INSERT = VK_INSERT;
    constexpr int DELETE_KEY = VK_DELETE;
    constexpr int HOME = VK_HOME;
    constexpr int END = VK_END;
    constexpr int PAGE_UP = VK_PRIOR;
    constexpr int PAGE_DOWN = VK_NEXT;
    constexpr int UP = VK_UP;
    constexpr int DOWN = VK_DOWN;
    constexpr int LEFT = VK_LEFT;
    constexpr int RIGHT = VK_RIGHT;
    constexpr int PRINT_SCREEN = VK_SNAPSHOT;
    constexpr int SCROLL_LOCK = VK_SCROLL;
    constexpr int PAUSE = VK_PAUSE;
    constexpr int NUM_LOCK = VK_NUMLOCK;

    // Additional keys
    constexpr int SEMICOLON = VK_OEM_1;     // ; :
    constexpr int EQUALS = VK_OEM_PLUS;     // = +
    constexpr int COMMA = VK_OEM_COMMA;     // , <
    constexpr int MINUS = VK_OEM_MINUS;     // - _
    constexpr int PERIOD = VK_OEM_PERIOD;   // . >
    constexpr int SLASH = VK_OEM_2;         // / ?
    constexpr int GRAVE = VK_OEM_3;         // ` ~
    constexpr int LBRACKET = VK_OEM_4;      // [ {
    constexpr int BACKSLASH = VK_OEM_5;     // \ |
    constexpr int RBRACKET = VK_OEM_6;      // ] }
    constexpr int QUOTE = VK_OEM_7;         // ' "

    // Numpad keys
    constexpr int NUMPAD_0 = VK_NUMPAD0;
    constexpr int NUMPAD_1 = VK_NUMPAD1;
    constexpr int NUMPAD_2 = VK_NUMPAD2;
    constexpr int NUMPAD_3 = VK_NUMPAD3;
    constexpr int NUMPAD_4 = VK_NUMPAD4;
    constexpr int NUMPAD_5 = VK_NUMPAD5;
    constexpr int NUMPAD_6 = VK_NUMPAD6;
    constexpr int NUMPAD_7 = VK_NUMPAD7;
    constexpr int NUMPAD_8 = VK_NUMPAD8;
    constexpr int NUMPAD_9 = VK_NUMPAD9;
    constexpr int NUMPAD_MULTIPLY = VK_MULTIPLY;
    constexpr int NUMPAD_ADD = VK_ADD;
    constexpr int NUMPAD_SUBTRACT = VK_SUBTRACT;
    constexpr int NUMPAD_DECIMAL = VK_DECIMAL;
    constexpr int NUMPAD_DIVIDE = VK_DIVIDE;
    constexpr int NUMPAD_SEPARATOR = VK_SEPARATOR;

    // Modifier key masks for combination detection
    constexpr int SHIFT_MASK = 0x01;
    constexpr int CTRL_MASK = 0x02;
    constexpr int ALT_MASK = 0x04;
    constexpr int WIN_MASK = 0x08;
}

// Mouse button constants
namespace MouseButton
{
    constexpr int LEFT = 0;
    constexpr int RIGHT = 1;
    constexpr int MIDDLE = 2;
    constexpr int X1 = 3;
    constexpr int X2 = 4;
}

// Gamepad button constants
namespace GamepadButton
{
    constexpr int A = 0;
    constexpr int B = 1;
    constexpr int X = 2;
    constexpr int Y = 3;
    constexpr int LEFT_SHOULDER = 4;
    constexpr int RIGHT_SHOULDER = 5;
    constexpr int BACK = 6;
    constexpr int START = 7;
    constexpr int LEFT_THUMB = 8;
    constexpr int RIGHT_THUMB = 9;
    constexpr int DPAD_UP = 10;
    constexpr int DPAD_RIGHT = 11;
    constexpr int DPAD_DOWN = 12;
    constexpr int DPAD_LEFT = 13;
}

// Gamepad axis constants
namespace GamepadAxis
{
    constexpr int LEFT_X = 0;
    constexpr int LEFT_Y = 1;
    constexpr int RIGHT_X = 2;
    constexpr int RIGHT_Y = 3;
    constexpr int LEFT_TRIGGER = 4;
    constexpr int RIGHT_TRIGGER = 5;
}

// Structure to store keyboard state for a single key
struct KeyState
{
    bool down = false;         // Key is currently down
    bool downPrevious = false; // Key was down last frame
    std::string name;          // Human-readable name of the key
};

// Structure for key combination (main key + modifiers)
struct KeyCombo
{
    int keyCode;
    int modifiers;  // Bitmask of modifiers (SHIFT, CTRL, ALT, etc.)

    // Comparison operators for use in maps and sets
    bool operator==(const KeyCombo& other) const {
        return keyCode == other.keyCode && modifiers == other.modifiers;
    }

    bool operator<(const KeyCombo& other) const {
        if (keyCode != other.keyCode)
            return keyCode < other.keyCode;
        return modifiers < other.modifiers;
    }
};

// Hash function for KeyCombo to use in unordered containers
namespace std {
    template<>
    struct hash<KeyCombo> {
        size_t operator()(const KeyCombo& k) const {
            return std::hash<int>()(k.keyCode) ^ (std::hash<int>()(k.modifiers) << 1);
        }
    };
}

// Structure to store mouse state
struct MouseState
{
    int x = 0;                 // Current X position
    int y = 0;                 // Current Y position
    int prevX = 0;             // Previous X position
    int prevY = 0;             // Previous Y position
    int deltaX = 0;            // Change in X position
    int deltaY = 0;            // Change in Y position
    float wheelDelta = 0.0f;   // Change in mouse wheel
    bool buttons[5] = { false }; // Button states (left, right, middle, X1, X2)
    bool buttonsPrev[5] = { false }; // Previous button states
};

// Structure to store gamepad state
struct GamepadState
{
    bool connected = false;    // Whether the gamepad is connected
    float leftStickX = 0.0f;   // Left stick X-axis (-1.0 to 1.0)
    float leftStickY = 0.0f;   // Left stick Y-axis (-1.0 to 1.0)
    float rightStickX = 0.0f;  // Right stick X-axis (-1.0 to 1.0)
    float rightStickY = 0.0f;  // Right stick Y-axis (-1.0 to 1.0)
    float leftTrigger = 0.0f;  // Left trigger (0.0 to 1.0)
    float rightTrigger = 0.0f; // Right trigger (0.0 to 1.0)
    bool buttons[14] = { false }; // Button states
    bool buttonsPrev[14] = { false }; // Previous button states
};

class InputHandler
{
public:
    InputHandler();
    ~InputHandler();

    // Initialization
    void Initialize(HWND hwnd);

    // Call once per frame to update all input states
    void Update();

    // Process Windows messages for input
    void ProcessMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Keyboard functions
    bool IsKeyDown(int keyCode) const;
    bool IsKeyPressed(int keyCode) const;   // Key was just pressed
    bool IsKeyReleased(int keyCode) const;  // Key was just released
    const std::string& GetKeyName(int keyCode) const;

    // Modifier combination functions
    bool IsModifierDown(int modifierMask) const;  // Check if specified modifier(s) is down
    bool IsKeyComboDown(int keyCode, int modifierMask) const; // Check if key+modifier combo is down
    bool IsKeyComboPressed(int keyCode, int modifierMask) const; // Check if key+modifier combo was just pressed
    bool IsKeyComboReleased(int keyCode, int modifierMask) const; // Check if key+modifier combo was just released

    // Shift+Key shorthand functions (most common use case)
    bool IsShiftKeyDown(int keyCode) const;
    bool IsShiftKeyPressed(int keyCode) const;
    bool IsShiftKeyReleased(int keyCode) const;

    // Mouse functions
    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonReleased(int button) const;
    int GetMouseX() const;
    int GetMouseY() const;
    int GetMouseDeltaX() const;
    int GetMouseDeltaY() const;
    float GetMouseWheelDelta() const;

    // Gamepad functions
    bool IsGamepadAvailable(int gamepadIndex = 0) const;
    bool IsGamepadButtonDown(int gamepadIndex, int button) const;
    bool IsGamepadButtonPressed(int gamepadIndex, int button) const;
    bool IsGamepadButtonReleased(int gamepadIndex, int button) const;
    float GetGamepadAxisValue(int gamepadIndex, int axis) const;
    const char* GetGamepadName(int gamepadIndex = 0) const;

    // Get current state
    const MouseState& GetMouseState() const;
    const GamepadState& GetGamepadState(int gamepadIndex = 0) const;

    // Get modifiers state
    bool IsShiftDown() const;
    bool IsCtrlDown() const;
    bool IsAltDown() const;

    // Register a key combo action with a name
    void RegisterKeyCombo(const std::string& name, int keyCode, int modifiers);

    // Check if a named combo is active
    bool IsComboDown(const std::string& name) const;
    bool IsComboPressed(const std::string& name) const;

private:
    // Helper methods for initialization
    void InitializeKeyboardMap();

    // Helper methods for updates
    void UpdateKeyboardState();
    void UpdateMouseState();
    void UpdateGamepadState();
    void UpdateModifierState();

    // Utility function to get current modifier state
    int GetCurrentModifiers() const;

    // Map key code to key state
    std::map<int, KeyState> m_KeyboardMap;

    // Keep track of previous frame key combo states
    std::unordered_set<KeyCombo> m_ActiveKeyCombos;
    std::unordered_set<KeyCombo> m_PrevActiveKeyCombos;

    // Named key combos
    std::map<std::string, KeyCombo> m_RegisteredCombos;

    // Current modifier state
    int m_CurrentModifiers = 0;
    int m_PreviousModifiers = 0;

    // Mouse state
    MouseState m_MouseState;

    // Gamepad states (support for 4 gamepads)
    std::array<GamepadState, 4> m_GamepadStates;

    // Window handle for mouse position calculations
    HWND m_Hwnd;

    // Key name map with default "Unknown"
    std::string m_UnknownKey = "Unknown";
};