#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <raylib.h>
#include <map>
#include <vector>
#include <string>

// Structure to store input state
struct InputState {
    bool isPressed;
    const char* name;
};

// Character key with normal and shifted representations
struct CharKey {
    int keyCode;
    const char* normalName;
    const char* shiftedName;
};

class InputHandler {
public:
    InputHandler();
    void Update();

    // Getters for input states
    const std::map<int, InputState>& GetKeyboardMap() const;
    const std::map<int, std::pair<const char*, const char*>>& GetCharKeyMap() const;
    const std::map<int, InputState>& GetMouseMap() const;
    const std::map<int, InputState>& GetGamepadMap() const;
    bool IsMouseWheelUp() const;
    bool IsMouseWheelDown() const;
    float GetLeftTrigger() const;
    float GetRightTrigger() const;
    bool IsShiftPressed() const;
    bool IsGamepadAvailable() const;
    const char* GetGamepadName() const;

    // Gamepad axis movements
    float GetGamepadAxisLeftX() const;
    float GetGamepadAxisLeftY() const;
    float GetGamepadAxisRightX() const;
    float GetGamepadAxisRightY() const;

private:
    // Input states
    std::map<int, InputState> keyboardMap;
    std::map<int, std::pair<const char*, const char*>> charKeyMap;
    std::map<int, InputState> mouseMap;
    std::map<int, InputState> gamepadMap;

    // Custom mouse wheel states
    bool mouseWheelUp;
    bool mouseWheelDown;

    // Gamepad trigger states (analog)
    float leftTrigger;
    float rightTrigger;

    // Gamepad detection
    int gamepadIndex;

    // Initialize input maps
    void InitializeKeyboardMap();
    void InitializeCharKeyMap();
    void InitializeMouseMap();
    void InitializeGamepadMap();
};

#endif // INPUT_HANDLER_H