#include "InputHandler.h"

InputHandler::InputHandler() :
    mouseWheelUp(false),
    mouseWheelDown(false),
    leftTrigger(0.0f),
    rightTrigger(0.0f),
    gamepadIndex(0) {

    InitializeKeyboardMap();
    InitializeCharKeyMap();
    InitializeMouseMap();
    InitializeGamepadMap();
}

void InputHandler::InitializeKeyboardMap() {
    // Special keys
    std::vector<std::pair<int, const char*>> specialKeys = {
        {KEY_SPACE, "SPACE"},
        {KEY_ESCAPE, "ESC"},
        {KEY_ENTER, "ENTER"},
        {KEY_TAB, "TAB"},
        {KEY_BACKSPACE, "BACK"},
        {KEY_INSERT, "INS"},
        {KEY_DELETE, "DEL"},
        {KEY_RIGHT, "RIGHT"},
        {KEY_LEFT, "LEFT"},
        {KEY_DOWN, "DOWN"},
        {KEY_UP, "UP"},
        {KEY_PAGE_UP, "PG UP"},
        {KEY_PAGE_DOWN, "PG DN"},
        {KEY_HOME, "HOME"},
        {KEY_END, "END"},
        {KEY_CAPS_LOCK, "CAPS"},
        {KEY_SCROLL_LOCK, "SCRL"},
        {KEY_NUM_LOCK, "NUM"},
        {KEY_PRINT_SCREEN, "PRNT"},
        {KEY_PAUSE, "PAUSE"},
        {KEY_F1, "F1"},
        {KEY_F2, "F2"},
        {KEY_F3, "F3"},
        {KEY_F4, "F4"},
        {KEY_F5, "F5"},
        {KEY_F6, "F6"},
        {KEY_F7, "F7"},
        {KEY_F8, "F8"},
        {KEY_F9, "F9"},
        {KEY_F10, "F10"},
        {KEY_F11, "F11"},
        {KEY_F12, "F12"},
        {KEY_LEFT_SHIFT, "L SHFT"},
        {KEY_LEFT_CONTROL, "L CTRL"},
        {KEY_LEFT_ALT, "L ALT"},
        {KEY_LEFT_SUPER, "L WIN"},
        {KEY_RIGHT_SHIFT, "R SHFT"},
        {KEY_RIGHT_CONTROL, "R CTRL"},
        {KEY_RIGHT_ALT, "R ALT"},
        {KEY_RIGHT_SUPER, "R WIN"},
        {KEY_KB_MENU, "MENU"},
        {KEY_KP_0, "KP 0"},
        {KEY_KP_1, "KP 1"},
        {KEY_KP_2, "KP 2"},
        {KEY_KP_3, "KP 3"},
        {KEY_KP_4, "KP 4"},
        {KEY_KP_5, "KP 5"},
        {KEY_KP_6, "KP 6"},
        {KEY_KP_7, "KP 7"},
        {KEY_KP_8, "KP 8"},
        {KEY_KP_9, "KP 9"},
        {KEY_KP_DECIMAL, "KP ."},
        {KEY_KP_DIVIDE, "KP /"},
        {KEY_KP_MULTIPLY, "KP *"},
        {KEY_KP_SUBTRACT, "KP -"},
        {KEY_KP_ADD, "KP +"},
        {KEY_KP_ENTER, "KP ENT"},
        {KEY_KP_EQUAL, "KP ="}
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
    // Mouse buttons
    mouseMap = {
        {MOUSE_BUTTON_LEFT, {false, "Left Click"}},
        {MOUSE_BUTTON_RIGHT, {false, "Right Click"}},
        {MOUSE_BUTTON_MIDDLE, {false, "Middle Click"}},
        {MOUSE_BUTTON_SIDE, {false, "Side Button 1"}},
        {MOUSE_BUTTON_EXTRA, {false, "Side Button 2"}}
    };
}

void InputHandler::InitializeGamepadMap() {
    // Gamepad buttons for Xbox One controller
    gamepadMap = {
        // Face buttons
        {GAMEPAD_BUTTON_RIGHT_FACE_DOWN, {false, "A Button"}},
        {GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, {false, "B Button"}},
        {GAMEPAD_BUTTON_RIGHT_FACE_LEFT, {false, "X Button"}},
        {GAMEPAD_BUTTON_RIGHT_FACE_UP, {false, "Y Button"}},

        // D-Pad
        {GAMEPAD_BUTTON_LEFT_FACE_UP, {false, "D-Pad Up"}},
        {GAMEPAD_BUTTON_LEFT_FACE_RIGHT, {false, "D-Pad Right"}},
        {GAMEPAD_BUTTON_LEFT_FACE_DOWN, {false, "D-Pad Down"}},
        {GAMEPAD_BUTTON_LEFT_FACE_LEFT, {false, "D-Pad Left"}},

        // Shoulder buttons
        {GAMEPAD_BUTTON_LEFT_TRIGGER_1, {false, "L Shoulder"}},
        {GAMEPAD_BUTTON_RIGHT_TRIGGER_1, {false, "R Shoulder"}},

        // Other buttons
        {GAMEPAD_BUTTON_MIDDLE_LEFT, {false, "Back/View"}},
        {GAMEPAD_BUTTON_MIDDLE, {false, "Xbox Button"}},
        {GAMEPAD_BUTTON_MIDDLE_RIGHT, {false, "Start/Menu"}},
        {GAMEPAD_BUTTON_LEFT_THUMB, {false, "L Stick Press"}},
        {GAMEPAD_BUTTON_RIGHT_THUMB, {false, "R Stick Press"}}
    };
}

void InputHandler::Update() {
    // Check if either shift key is pressed
    bool shiftPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

    // Update keyboard states
    for (auto& key : keyboardMap) {
        key.second.isPressed = IsKeyDown(key.first);

        // Update character key display based on shift state
        auto charIt = charKeyMap.find(key.first);
        if (charIt != charKeyMap.end()) {
            // If shift is pressed, use the shifted name
            key.second.name = shiftPressed ? charIt->second.second : charIt->second.first;
        }
    }

    // Update mouse button states
    for (auto& button : mouseMap) {
        button.second.isPressed = IsMouseButtonDown(button.first);
    }

    // Update mouse wheel state
    float wheelMove = GetMouseWheelMove();
    mouseWheelUp = wheelMove > 0;
    mouseWheelDown = wheelMove < 0;

    // Update gamepad states if available
    if (IsGamepadAvailable()) {
        // Update gamepad button states
        for (auto& button : gamepadMap) {
            button.second.isPressed = IsGamepadButtonDown(gamepadIndex, button.first);
        }

        // Update trigger states
        leftTrigger = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_TRIGGER);
        rightTrigger = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_TRIGGER);
    }
}

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
    return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
}

bool InputHandler::IsGamepadAvailable() const {
    return ::IsGamepadAvailable(gamepadIndex);
}

const char* InputHandler::GetGamepadName() const {
    if (IsGamepadAvailable()) {
        return ::GetGamepadName(gamepadIndex);
    }
    return "No Gamepad";
}

float InputHandler::GetGamepadAxisLeftX() const {
    return IsGamepadAvailable() ? GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_X) : 0.0f;
}

float InputHandler::GetGamepadAxisLeftY() const {
    return IsGamepadAvailable() ? GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_Y) : 0.0f;
}

float InputHandler::GetGamepadAxisRightX() const {
    return IsGamepadAvailable() ? GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_X) : 0.0f;
}

float InputHandler::GetGamepadAxisRightY() const {
    return IsGamepadAvailable() ? GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_Y) : 0.0f;
}