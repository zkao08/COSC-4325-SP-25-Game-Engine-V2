// InputLuaAPI.cpp - Fixed version
#include "InputLuaAPI.h"
#include <iostream>

namespace InputLuaAPI
{
    void Initialize(sol::state& lua)
    {
        // Get singleton instance
        InputHandler& inputManager = InputHandler::GetInstance();

        // Create the Input API table
        sol::table inputAPI = lua["Input"].get_or_create<sol::table>();

        // Create KeyCode table in a more direct way
        sol::table keyCode = inputAPI["KeyCode"].get_or_create<sol::table>();
        
        // Letters
        keyCode["A"] = KeyCode::A;
        keyCode["B"] = KeyCode::B;
        keyCode["C"] = KeyCode::C;
        keyCode["D"] = KeyCode::D;
        keyCode["E"] = KeyCode::E;
        keyCode["F"] = KeyCode::F;
        keyCode["G"] = KeyCode::G;
        keyCode["H"] = KeyCode::H;
        keyCode["I"] = KeyCode::I;
        keyCode["J"] = KeyCode::J;
        keyCode["K"] = KeyCode::K;
        keyCode["L"] = KeyCode::L;
        keyCode["M"] = KeyCode::M;
        keyCode["N"] = KeyCode::N;
        keyCode["O"] = KeyCode::O;
        keyCode["P"] = KeyCode::P;
        keyCode["Q"] = KeyCode::Q;
        keyCode["R"] = KeyCode::R;
        keyCode["S"] = KeyCode::S;
        keyCode["T"] = KeyCode::T;
        keyCode["U"] = KeyCode::U;
        keyCode["V"] = KeyCode::V;
        keyCode["W"] = KeyCode::W;
        keyCode["X"] = KeyCode::X;
        keyCode["Y"] = KeyCode::Y;
        keyCode["Z"] = KeyCode::Z;
        
        // Numbers
        keyCode["NUM_0"] = KeyCode::NUM_0;
        keyCode["NUM_1"] = KeyCode::NUM_1;
        keyCode["NUM_2"] = KeyCode::NUM_2;
        keyCode["NUM_3"] = KeyCode::NUM_3;
        keyCode["NUM_4"] = KeyCode::NUM_4;
        keyCode["NUM_5"] = KeyCode::NUM_5;
        keyCode["NUM_6"] = KeyCode::NUM_6;
        keyCode["NUM_7"] = KeyCode::NUM_7;
        keyCode["NUM_8"] = KeyCode::NUM_8;
        keyCode["NUM_9"] = KeyCode::NUM_9;
        
        // Function keys
        keyCode["F1"] = KeyCode::F1;
        keyCode["F2"] = KeyCode::F2;
        keyCode["F3"] = KeyCode::F3;
        keyCode["F4"] = KeyCode::F4;
        keyCode["F5"] = KeyCode::F5;
        keyCode["F6"] = KeyCode::F6;
        keyCode["F7"] = KeyCode::F7;
        keyCode["F8"] = KeyCode::F8;
        keyCode["F9"] = KeyCode::F9;
        keyCode["F10"] = KeyCode::F10;
        keyCode["F11"] = KeyCode::F11;
        keyCode["F12"] = KeyCode::F12;
        
        // Special keys
        keyCode["ESCAPE"] = KeyCode::ESCAPE;
        keyCode["SPACE"] = KeyCode::SPACE;
        keyCode["ENTER"] = KeyCode::ENTER;
        keyCode["BACKSPACE"] = KeyCode::BACKSPACE;
        keyCode["TAB"] = KeyCode::TAB;
        keyCode["CAPS_LOCK"] = KeyCode::CAPS_LOCK;
        keyCode["SHIFT_LEFT"] = KeyCode::SHIFT_LEFT;
        keyCode["SHIFT_RIGHT"] = KeyCode::SHIFT_RIGHT;
        keyCode["CONTROL_LEFT"] = KeyCode::CONTROL_LEFT;
        keyCode["CONTROL_RIGHT"] = KeyCode::CONTROL_RIGHT;
        keyCode["ALT_LEFT"] = KeyCode::ALT_LEFT;
        keyCode["ALT_RIGHT"] = KeyCode::ALT_RIGHT;
        keyCode["SUPER_LEFT"] = KeyCode::SUPER_LEFT;
        keyCode["SUPER_RIGHT"] = KeyCode::SUPER_RIGHT;
        keyCode["MENU"] = KeyCode::MENU;
        keyCode["INSERT"] = KeyCode::INSERT;
        keyCode["DELETE_KEY"] = KeyCode::DELETE_KEY;
        keyCode["HOME"] = KeyCode::HOME;
        keyCode["END"] = KeyCode::END;
        keyCode["PAGE_UP"] = KeyCode::PAGE_UP;
        keyCode["PAGE_DOWN"] = KeyCode::PAGE_DOWN;
        keyCode["UP"] = KeyCode::UP;
        keyCode["DOWN"] = KeyCode::DOWN;
        keyCode["LEFT"] = KeyCode::LEFT;
        keyCode["RIGHT"] = KeyCode::RIGHT;
        keyCode["PRINT_SCREEN"] = KeyCode::PRINT_SCREEN;
        keyCode["SCROLL_LOCK"] = KeyCode::SCROLL_LOCK;
        keyCode["PAUSE"] = KeyCode::PAUSE;
        keyCode["NUM_LOCK"] = KeyCode::NUM_LOCK;
        
        // Additional keys
        keyCode["SEMICOLON"] = KeyCode::SEMICOLON;
        keyCode["EQUALS"] = KeyCode::EQUALS;
        keyCode["COMMA"] = KeyCode::COMMA;
        keyCode["MINUS"] = KeyCode::MINUS;
        keyCode["PERIOD"] = KeyCode::PERIOD;
        keyCode["SLASH"] = KeyCode::SLASH;
        keyCode["GRAVE"] = KeyCode::GRAVE;
        keyCode["LBRACKET"] = KeyCode::LBRACKET;
        keyCode["BACKSLASH"] = KeyCode::BACKSLASH;
        keyCode["RBRACKET"] = KeyCode::RBRACKET;
        keyCode["QUOTE"] = KeyCode::QUOTE;
        
        // Numpad keys
        keyCode["NUMPAD_0"] = KeyCode::NUMPAD_0;
        keyCode["NUMPAD_1"] = KeyCode::NUMPAD_1;
        keyCode["NUMPAD_2"] = KeyCode::NUMPAD_2;
        keyCode["NUMPAD_3"] = KeyCode::NUMPAD_3;
        keyCode["NUMPAD_4"] = KeyCode::NUMPAD_4;
        keyCode["NUMPAD_5"] = KeyCode::NUMPAD_5;
        keyCode["NUMPAD_6"] = KeyCode::NUMPAD_6;
        keyCode["NUMPAD_7"] = KeyCode::NUMPAD_7;
        keyCode["NUMPAD_8"] = KeyCode::NUMPAD_8;
        keyCode["NUMPAD_9"] = KeyCode::NUMPAD_9;
        keyCode["NUMPAD_MULTIPLY"] = KeyCode::NUMPAD_MULTIPLY;
        keyCode["NUMPAD_ADD"] = KeyCode::NUMPAD_ADD;
        keyCode["NUMPAD_SUBTRACT"] = KeyCode::NUMPAD_SUBTRACT;
        keyCode["NUMPAD_DECIMAL"] = KeyCode::NUMPAD_DECIMAL;
        keyCode["NUMPAD_DIVIDE"] = KeyCode::NUMPAD_DIVIDE;
        keyCode["NUMPAD_SEPARATOR"] = KeyCode::NUMPAD_SEPARATOR;

        // Constants for MouseButton
        sol::table mouseButton = inputAPI["MouseButton"].get_or_create<sol::table>();
        mouseButton["LEFT"] = MouseButton::LEFT;
        mouseButton["RIGHT"] = MouseButton::RIGHT;
        mouseButton["MIDDLE"] = MouseButton::MIDDLE;
        mouseButton["X1"] = MouseButton::X1;
        mouseButton["X2"] = MouseButton::X2;

        // Constants for GamepadButton
        sol::table gamepadButton = inputAPI["GamepadButton"].get_or_create<sol::table>();
        gamepadButton["A"] = GamepadButton::A;
        gamepadButton["B"] = GamepadButton::B;
        gamepadButton["X"] = GamepadButton::X;
        gamepadButton["Y"] = GamepadButton::Y;
        gamepadButton["LEFT_SHOULDER"] = GamepadButton::LEFT_SHOULDER;
        gamepadButton["RIGHT_SHOULDER"] = GamepadButton::RIGHT_SHOULDER;
        gamepadButton["BACK"] = GamepadButton::BACK;
        gamepadButton["START"] = GamepadButton::START;
        gamepadButton["LEFT_THUMB"] = GamepadButton::LEFT_THUMB;
        gamepadButton["RIGHT_THUMB"] = GamepadButton::RIGHT_THUMB;
        gamepadButton["DPAD_UP"] = GamepadButton::DPAD_UP;
        gamepadButton["DPAD_RIGHT"] = GamepadButton::DPAD_RIGHT;
        gamepadButton["DPAD_DOWN"] = GamepadButton::DPAD_DOWN;
        gamepadButton["DPAD_LEFT"] = GamepadButton::DPAD_LEFT;

        // Constants for GamepadAxis
        sol::table gamepadAxis = inputAPI["GamepadAxis"].get_or_create<sol::table>();
        gamepadAxis["LEFT_X"] = GamepadAxis::LEFT_X;
        gamepadAxis["LEFT_Y"] = GamepadAxis::LEFT_Y;
        gamepadAxis["RIGHT_X"] = GamepadAxis::RIGHT_X;
        gamepadAxis["RIGHT_Y"] = GamepadAxis::RIGHT_Y;
        gamepadAxis["LEFT_TRIGGER"] = GamepadAxis::LEFT_TRIGGER;
        gamepadAxis["RIGHT_TRIGGER"] = GamepadAxis::RIGHT_TRIGGER;

        // Constants for Modifier masks
        sol::table modifiers = inputAPI["Modifiers"].get_or_create<sol::table>();
        modifiers["SHIFT"] = KeyCode::SHIFT_MASK;
        modifiers["CTRL"] = KeyCode::CTRL_MASK;
        modifiers["ALT"] = KeyCode::ALT_MASK;
        modifiers["WIN"] = KeyCode::WIN_MASK;

        // Register keyboard functions
        inputAPI["isKeyDown"] = [&inputManager](int keyCode) {
            return inputManager.IsKeyDown(keyCode);
        };

        inputAPI["isKeyPressed"] = [&inputManager](int keyCode) {
            return inputManager.IsKeyPressed(keyCode);
        };

        inputAPI["isKeyReleased"] = [&inputManager](int keyCode) {
            return inputManager.IsKeyReleased(keyCode);
        };

        inputAPI["getKeyName"] = [&inputManager](int keyCode) {
            return inputManager.GetKeyName(keyCode);
        };

        // Register key combo functions
        inputAPI["isKeyComboDown"] = [&inputManager](int keyCode, int modifierMask) {
            return inputManager.IsKeyComboDown(keyCode, modifierMask);
        };

        inputAPI["isKeyComboPressed"] = [&inputManager](int keyCode, int modifierMask) {
            return inputManager.IsKeyComboPressed(keyCode, modifierMask);
        };

        inputAPI["isKeyComboReleased"] = [&inputManager](int keyCode, int modifierMask) {
            return inputManager.IsKeyComboReleased(keyCode, modifierMask);
        };

        // Register shorthand functions for Shift combos
        inputAPI["isShiftKeyDown"] = [&inputManager](int keyCode) {
            return inputManager.IsShiftKeyDown(keyCode);
        };

        inputAPI["isShiftKeyPressed"] = [&inputManager](int keyCode) {
            return inputManager.IsShiftKeyPressed(keyCode);
        };

        inputAPI["isShiftKeyReleased"] = [&inputManager](int keyCode) {
            return inputManager.IsShiftKeyReleased(keyCode);
        };

        // Register mouse functions
        inputAPI["isMouseButtonDown"] = [&inputManager](int button) {
            return inputManager.IsMouseButtonDown(button);
        };

        inputAPI["isMouseButtonPressed"] = [&inputManager](int button) {
            return inputManager.IsMouseButtonPressed(button);
        };

        inputAPI["isMouseButtonReleased"] = [&inputManager](int button) {
            return inputManager.IsMouseButtonReleased(button);
        };

        inputAPI["getMousePosition"] = [&inputManager, &lua]() -> sol::table {
            sol::table position = lua.create_table();
            position["x"] = inputManager.GetMouseX();
            position["y"] = inputManager.GetMouseY();
            return position;
        };

        inputAPI["getMouseDelta"] = [&inputManager, &lua]() -> sol::table {
            sol::table delta = lua.create_table();
            delta["x"] = inputManager.GetMouseDeltaX();
            delta["y"] = inputManager.GetMouseDeltaY();
            return delta;
        };

        inputAPI["getMouseWheelDelta"] = [&inputManager]() {
            return inputManager.GetMouseWheelDelta();
        };

        // Register gamepad functions
        inputAPI["isGamepadAvailable"] = sol::overload(
            [&inputManager]() {
                return inputManager.IsGamepadAvailable();
            },
            [&inputManager](int gamepadIndex) {
                return inputManager.IsGamepadAvailable(gamepadIndex);
            }
        );

        inputAPI["isGamepadButtonDown"] = [&inputManager](int gamepadIndex, int button) {
            return inputManager.IsGamepadButtonDown(gamepadIndex, button);
        };

        inputAPI["isGamepadButtonPressed"] = [&inputManager](int gamepadIndex, int button) {
            return inputManager.IsGamepadButtonPressed(gamepadIndex, button);
        };

        inputAPI["isGamepadButtonReleased"] = [&inputManager](int gamepadIndex, int button) {
            return inputManager.IsGamepadButtonReleased(gamepadIndex, button);
        };

        inputAPI["getGamepadAxisValue"] = [&inputManager](int gamepadIndex, int axis) {
            return inputManager.GetGamepadAxisValue(gamepadIndex, axis);
        };

        inputAPI["getGamepadName"] = sol::overload(
            [&inputManager]() {
                return inputManager.GetGamepadName();
            },
            [&inputManager](int gamepadIndex) {
                return inputManager.GetGamepadName(gamepadIndex);
            }
        );

        // Register modifier state functions
        inputAPI["isShiftDown"] = [&inputManager]() {
            return inputManager.IsShiftDown();
        };

        inputAPI["isCtrlDown"] = [&inputManager]() {
            return inputManager.IsCtrlDown();
        };

        inputAPI["isAltDown"] = [&inputManager]() {
            return inputManager.IsAltDown();
        };

        // Named combo functions
        inputAPI["registerKeyCombo"] = [&inputManager](const std::string& name, int keyCode, int modifiers) {
            inputManager.RegisterKeyCombo(name, keyCode, modifiers);
        };

        inputAPI["isComboDown"] = [&inputManager](const std::string& name) {
            return inputManager.IsComboDown(name);
        };

        inputAPI["isComboPressed"] = [&inputManager](const std::string& name) {
            return inputManager.IsComboPressed(name);
        };

        // Get full state objects
        inputAPI["getMouseState"] = [&inputManager, &lua]() -> sol::table {
            const MouseState& state = inputManager.GetMouseState();
            sol::table result = lua.create_table();
            
            result["x"] = state.x;
            result["y"] = state.y;
            result["deltaX"] = state.deltaX;
            result["deltaY"] = state.deltaY;
            result["wheelDelta"] = state.wheelDelta;
            
            sol::table buttons = lua.create_table();
            for (int i = 0; i < 5; i++) {
                buttons[i+1] = state.buttons[i]; // Lua uses 1-based indexing
            }
            result["buttons"] = buttons;
            
            return result;
        };

        inputAPI["getGamepadState"] = [&inputManager, &lua](int gamepadIndex) -> sol::table {
            const GamepadState& state = inputManager.GetGamepadState(gamepadIndex);
            sol::table result = lua.create_table();
            
            result["connected"] = state.connected;
            result["leftStickX"] = state.leftStickX;
            result["leftStickY"] = state.leftStickY;
            result["rightStickX"] = state.rightStickX;
            result["rightStickY"] = state.rightStickY;
            result["leftTrigger"] = state.leftTrigger;
            result["rightTrigger"] = state.rightTrigger;
            
            sol::table buttons = lua.create_table();
            for (int i = 0; i < 14; i++) {
                buttons[i+1] = state.buttons[i]; // Lua uses 1-based indexing
            }
            result["buttons"] = buttons;
            
            return result;
        };

        std::cout << "Input Lua API initialized successfully" << std::endl;
    }
}