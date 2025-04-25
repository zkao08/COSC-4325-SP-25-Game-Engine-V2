#include <raylib.h>
#include "InputHandler.h"
#include <iostream>

// Implementation struct definition
struct InputHandler::Impl {
    bool initialized;
    bool rayWindowCreated;

    Impl() :
        initialized(false),
        rayWindowCreated(false)
    {
    }
};

InputHandler::InputHandler() :
    mouseWheelUp(false),
    mouseWheelDown(false),
    leftTrigger(0.0f),
    rightTrigger(0.0f),
    gamepadIndex(0),
    pImpl(new Impl())
{
    InitializeKeyboardMap();
    InitializeCharKeyMap();
    InitializeMouseMap();
    InitializeGamepadMap();
}

InputHandler::~InputHandler()
{
    Shutdown();
    delete pImpl;
}

bool InputHandler::IsKeyPressed(int keyCode) const
{
    if (!pImpl->initialized) return false;
    
    // Use raylib's IsKeyDown directly
    return ::IsKeyDown(keyCode);
}

bool InputHandler::Initialize()
{
    if (!pImpl) return false;
    if (pImpl->initialized) return true;

    try
    {
        // Create a very small window instead of a hidden one
        ::SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MINIMIZED);
        ::InitWindow(100, 100, "Input Window");
        pImpl->rayWindowCreated = true;

        // Set a target FPS to ensure proper event processing
        ::SetTargetFPS(60);

        pImpl->initialized = true;
        std::cout << "Input system initialized successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize input system: " << e.what() << std::endl;
        return false;
    }
}

void InputHandler::Shutdown()
{
    if (pImpl && pImpl->initialized)
    {
        if (pImpl->rayWindowCreated)
        {
            ::CloseWindow();
            pImpl->rayWindowCreated = false;
        }

        pImpl->initialized = false;
        std::cout << "Input system shut down" << std::endl;
    }
}

// Initialize maps functions using fully-qualified raylib values
void InputHandler::InitializeKeyboardMap() {
    // Special keys - using :: to make it clear these are from the global namespace
    std::vector<std::pair<int, const char*>> specialKeys = {
        {::KEY_SPACE, "SPACE"},
        {::KEY_ESCAPE, "ESC"},
        {::KEY_ENTER, "ENTER"},
        {::KEY_TAB, "TAB"},
        {::KEY_BACKSPACE, "BACK"},
        {::KEY_INSERT, "INS"},
        {::KEY_DELETE, "DEL"},
        {::KEY_RIGHT, "RIGHT"},
        {::KEY_LEFT, "LEFT"},
        {::KEY_DOWN, "DOWN"},
        {::KEY_UP, "UP"},
        {::KEY_PAGE_UP, "PG UP"},
        {::KEY_PAGE_DOWN, "PG DN"},
        {::KEY_HOME, "HOME"},
        {::KEY_END, "END"},
        {::KEY_CAPS_LOCK, "CAPS"},
        {::KEY_SCROLL_LOCK, "SCRL"},
        {::KEY_NUM_LOCK, "NUM"},
        {::KEY_PRINT_SCREEN, "PRNT"},
        {::KEY_PAUSE, "PAUSE"},
        {::KEY_F1, "F1"},
        {::KEY_F2, "F2"},
        {::KEY_F3, "F3"},
        {::KEY_F4, "F4"},
        {::KEY_F5, "F5"},
        {::KEY_F6, "F6"},
        {::KEY_F7, "F7"},
        {::KEY_F8, "F8"},
        {::KEY_F9, "F9"},
        {::KEY_F10, "F10"},
        {::KEY_F11, "F11"},
        {::KEY_F12, "F12"},
        {::KEY_LEFT_SHIFT, "L SHFT"},
        {::KEY_LEFT_CONTROL, "L CTRL"},
        {::KEY_LEFT_ALT, "L ALT"},
        {::KEY_LEFT_SUPER, "L WIN"},
        {::KEY_RIGHT_SHIFT, "R SHFT"},
        {::KEY_RIGHT_CONTROL, "R CTRL"},
        {::KEY_RIGHT_ALT, "R ALT"},
        {::KEY_RIGHT_SUPER, "R WIN"},
        {::KEY_KB_MENU, "MENU"},
        {::KEY_KP_0, "KP 0"},
        {::KEY_KP_1, "KP 1"},
        {::KEY_KP_2, "KP 2"},
        {::KEY_KP_3, "KP 3"},
        {::KEY_KP_4, "KP 4"},
        {::KEY_KP_5, "KP 5"},
        {::KEY_KP_6, "KP 6"},
        {::KEY_KP_7, "KP 7"},
        {::KEY_KP_8, "KP 8"},
        {::KEY_KP_9, "KP 9"},
        {::KEY_KP_DECIMAL, "KP ."},
        {::KEY_KP_DIVIDE, "KP /"},
        {::KEY_KP_MULTIPLY, "KP *"},
        {::KEY_KP_SUBTRACT, "KP -"},
        {::KEY_KP_ADD, "KP +"},
        {::KEY_KP_ENTER, "KP ENT"},
        {::KEY_KP_EQUAL, "KP ="}
    };

    // Add special keys to the keyboard map
    for (auto& key : specialKeys) {
        keyboardMap[key.first] = { false, key.second };
    }
}

void InputHandler::InitializeCharKeyMap() {
    // Define character keys with both non-shifted and shifted values
    std::vector<CharKey> characterKeys = {
        // Numbers and their shifted symbols
        {'0', "0", ")"},
        {'1', "1", "!"},
        {'2', "2", "@"},
        {'3', "3", "#"},
        {'4', "4", "$"},
        {'5', "5", "%"},
        {'6', "6", "^"},
        {'7', "7", "&"},
        {'8', "8", "*"},
        {'9', "9", "("},

        // Letters - will be shown as uppercase when shift is pressed
        {'A', "a", "A"},
        {'B', "b", "B"},
        {'C', "c", "C"},
        {'D', "d", "D"},
        {'E', "e", "E"},
        {'F', "f", "F"},
        {'G', "g", "G"},
        {'H', "h", "H"},
        {'I', "i", "I"},
        {'J', "j", "J"},
        {'K', "k", "K"},
        {'L', "l", "L"},
        {'M', "m", "M"},
        {'N', "n", "N"},
        {'O', "o", "O"},
        {'P', "p", "P"},
        {'Q', "q", "Q"},
        {'R', "r", "R"},
        {'S', "s", "S"},
        {'T', "t", "T"},
        {'U', "u", "U"},
        {'V', "v", "V"},
        {'W', "w", "W"},
        {'X', "x", "X"},
        {'Y', "y", "Y"},
        {'Z', "z", "Z"},

        // Symbols and their shifted variants
        {'`', "`", "~"},
        {'-', "-", "_"},
        {'=', "=", "+"},
        {'[', "[", "{"},
        {']', "]", "}"},
        {'\\', "\\", "|"},
        {';', ";", ":"},
        {'\'', "'", "\""},
        {',', ",", "<"},
        {'.', ".", ">"},
        {'/', "/", "?"}
    };

    // Add character keys to the map with both normal and shifted names
    for (auto& key : characterKeys) {
        charKeyMap[key.keyCode] = { key.normalName, key.shiftedName };
        // Initialize keyboardMap with normal names
        keyboardMap[key.keyCode] = { false, key.normalName };
    }
}

void InputHandler::InitializeMouseMap() {
    // Mouse buttons - fully qualify raylib constants
    mouseMap = {
        {::MOUSE_BUTTON_LEFT, {false, "Left Click"}},
        {::MOUSE_BUTTON_RIGHT, {false, "Right Click"}},
        {::MOUSE_BUTTON_MIDDLE, {false, "Middle Click"}},
        {::MOUSE_BUTTON_SIDE, {false, "Side Button 1"}},
        {::MOUSE_BUTTON_EXTRA, {false, "Side Button 2"}}
    };
}

void InputHandler::InitializeGamepadMap() {
    // Gamepad buttons for Xbox One controller - fully qualify raylib constants
    gamepadMap = {
        // Face buttons
        {::GAMEPAD_BUTTON_RIGHT_FACE_DOWN, {false, "A Button"}},
        {::GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, {false, "B Button"}},
        {::GAMEPAD_BUTTON_RIGHT_FACE_LEFT, {false, "X Button"}},
        {::GAMEPAD_BUTTON_RIGHT_FACE_UP, {false, "Y Button"}},

        // D-Pad
        {::GAMEPAD_BUTTON_LEFT_FACE_UP, {false, "D-Pad Up"}},
        {::GAMEPAD_BUTTON_LEFT_FACE_RIGHT, {false, "D-Pad Right"}},
        {::GAMEPAD_BUTTON_LEFT_FACE_DOWN, {false, "D-Pad Down"}},
        {::GAMEPAD_BUTTON_LEFT_FACE_LEFT, {false, "D-Pad Left"}},

        // Shoulder buttons
        {::GAMEPAD_BUTTON_LEFT_TRIGGER_1, {false, "L Shoulder"}},
        {::GAMEPAD_BUTTON_RIGHT_TRIGGER_1, {false, "R Shoulder"}},

        // Other buttons
        {::GAMEPAD_BUTTON_MIDDLE_LEFT, {false, "Back/View"}},
        {::GAMEPAD_BUTTON_MIDDLE, {false, "Xbox Button"}},
        {::GAMEPAD_BUTTON_MIDDLE_RIGHT, {false, "Start/Menu"}},
        {::GAMEPAD_BUTTON_LEFT_THUMB, {false, "L Stick Press"}},
        {::GAMEPAD_BUTTON_RIGHT_THUMB, {false, "R Stick Press"}}
    };
}

// Update function now checks for initialization and fully qualifies raylib function calls
void InputHandler::Update()
{
    if (!pImpl->initialized) return;

    // Run a full raylib frame to ensure input is processed
    ::BeginDrawing();
    ::EndDrawing();

    // Check if either shift key is pressed
    bool shiftPressed = ::IsKeyDown(::KEY_LEFT_SHIFT) || ::IsKeyDown(::KEY_RIGHT_SHIFT);

    // Update keyboard states
    for (auto& key : keyboardMap) {
        key.second.isPressed = ::IsKeyDown(key.first);

        // Update character key display based on shift state
        auto charIt = charKeyMap.find(key.first);
        if (charIt != charKeyMap.end()) {
            // If shift is pressed, use the shifted name
            key.second.name = shiftPressed ? charIt->second.second : charIt->second.first;
        }
    }

    // Update mouse button states
    for (auto& button : mouseMap) {
        button.second.isPressed = ::IsMouseButtonDown(button.first);
    }

    // Update mouse wheel state
    float wheelMove = ::GetMouseWheelMove();
    mouseWheelUp = wheelMove > 0;
    mouseWheelDown = wheelMove < 0;

    // Update gamepad states if available
    if (::IsGamepadAvailable(gamepadIndex)) {
        // Update gamepad button states
        for (auto& button : gamepadMap) {
            button.second.isPressed = ::IsGamepadButtonDown(gamepadIndex, button.first);
        }

        // Update trigger states
        leftTrigger = ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_LEFT_TRIGGER);
        rightTrigger = ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_RIGHT_TRIGGER);
    }
}

// Accessor methods - fully qualify raylib functions
const std::map<int, InputState>& InputHandler::GetKeyboardMap() const {
    return keyboardMap;
}

const std::map<int, std::pair<const char*, const char*>>& InputHandler::GetCharKeyMap() const {
    return charKeyMap;
}

const std::map<int, InputState>& InputHandler::GetMouseMap() const {
    return mouseMap;
}

const std::map<int, InputState>& InputHandler::GetGamepadMap() const {
    return gamepadMap;
}

bool InputHandler::IsMouseWheelUp() const {
    return mouseWheelUp;
}

bool InputHandler::IsMouseWheelDown() const {
    return mouseWheelDown;
}

float InputHandler::GetLeftTrigger() const {
    return leftTrigger;
}

float InputHandler::GetRightTrigger() const {
    return rightTrigger;
}

bool InputHandler::IsShiftPressed() const {
    if (!pImpl->initialized) return false;
    return ::IsKeyDown(::KEY_LEFT_SHIFT) || ::IsKeyDown(::KEY_RIGHT_SHIFT);
}

bool InputHandler::IsGamepadAvailable() const {
    if (!pImpl->initialized) return false;
    return ::IsGamepadAvailable(gamepadIndex);
}

const char* InputHandler::GetGamepadName() const {
    if (!pImpl->initialized || !IsGamepadAvailable()) {
        return "No Gamepad";
    }
    return ::GetGamepadName(gamepadIndex);
}

float InputHandler::GetGamepadAxisLeftX() const {
    if (!pImpl->initialized || !IsGamepadAvailable()) return 0.0f;
    return ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_LEFT_X);
}

float InputHandler::GetGamepadAxisLeftY() const {
    if (!pImpl->initialized || !IsGamepadAvailable()) return 0.0f;
    return ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_LEFT_Y);
}

float InputHandler::GetGamepadAxisRightX() const {
    if (!pImpl->initialized || !IsGamepadAvailable()) return 0.0f;
    return ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_RIGHT_X);
}

float InputHandler::GetGamepadAxisRightY() const {
    if (!pImpl->initialized || !IsGamepadAvailable()) return 0.0f;
    return ::GetGamepadAxisMovement(gamepadIndex, ::GAMEPAD_AXIS_RIGHT_Y);
}