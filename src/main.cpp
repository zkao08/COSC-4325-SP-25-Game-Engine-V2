#include <raylib.h>
#include <map>
#include <vector>
#include <string>

// Structure to store input state
struct InputState {
    bool isPressed;
    const char* name;
};

// Helper function to draw input status
void DrawInputStatus(const char* label, bool isPressed, int x, int y, int fontSize) {
    Color textColor = isPressed ? GREEN : GRAY;
    DrawText(label, x, y, fontSize, textColor);
}

int main() {
    // Initialize window with increased dimensions
    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Raylib Complete Input Monitor");
    SetTargetFPS(60);

    // Gamepad detection
    int gamepadIndex = 0;

    // Maps to store all keyboard key states
    std::map<int, InputState> keyboardMap;

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
        // Removed volume control keys
    };

    // Add special keys to the keyboard map
    for (auto& key : specialKeys) {
        keyboardMap[key.first] = { false, key.second };
    }

    // Define character keys with both non-shifted and shifted values
    struct CharKey {
        int keyCode;
        const char* normalName;
        const char* shiftedName;
    };

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

    // Map for character keys
    std::map<int, std::pair<const char*, const char*>> charKeyMap;

    // Add character keys to the map with both normal and shifted names
    for (auto& key : characterKeys) {
        charKeyMap[key.keyCode] = { key.normalName, key.shiftedName };
        // Initialize keyboardMap with normal names
        keyboardMap[key.keyCode] = { false, key.normalName };
    }

    // Mouse buttons
    std::map<int, InputState> mouseMap = {
        {MOUSE_BUTTON_LEFT, {false, "Left Click"}},
        {MOUSE_BUTTON_RIGHT, {false, "Right Click"}},
        {MOUSE_BUTTON_MIDDLE, {false, "Middle Click"}},
        {MOUSE_BUTTON_SIDE, {false, "Side Button 1"}},
        {MOUSE_BUTTON_EXTRA, {false, "Side Button 2"}}
    };

    // Custom mouse wheel states
    bool mouseWheelUp = false;
    bool mouseWheelDown = false;

    // Gamepad buttons for Xbox One controller
    std::map<int, InputState> gamepadMap = {
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

    // Gamepad trigger states (analog)
    float leftTrigger = 0.0f;
    float rightTrigger = 0.0f;

    // Main game loop
    while (!WindowShouldClose()) {
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
        bool gamepadAvailable = IsGamepadAvailable(gamepadIndex);

        if (gamepadAvailable) {
            // Update gamepad button states
            for (auto& button : gamepadMap) {
                button.second.isPressed = IsGamepadButtonDown(gamepadIndex, button.first);
            }

            // Update trigger states
            leftTrigger = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_TRIGGER);
            rightTrigger = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_TRIGGER);
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw mouse position
        Vector2 mousePos = GetMousePosition();
        DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", mousePos.x, mousePos.y), 10, 10, 24, DARKGRAY);

        // Draw shift status
        DrawText(TextFormat("Shift: %s", shiftPressed ? "PRESSED" : "NOT PRESSED"), 400, 10, 24,
            shiftPressed ? GREEN : DARKGRAY);

        // Draw GUI panels for different input devices
        int keyboardPanelWidth = 950;
        int keyboardPanelHeight = 580;
        int otherPanelWidth = 500;
        int gamepadPanelHeight = 700;
        int mousePanelHeight = 200;
        int fontSize = 18;

        // Panel for keyboard
        DrawRectangle(10, 40, keyboardPanelWidth, keyboardPanelHeight, Fade(LIGHTGRAY, 0.5f));
        DrawRectangleLines(10, 40, keyboardPanelWidth, keyboardPanelHeight, DARKGRAY);
        DrawText("KEYBOARD", 20, 50, 24, BLACK);

        // Draw alphabet, numbers, and symbols with bigger font
        DrawText("Letters & Numbers:", 20, 85, 20, DARKGRAY);

        // Grid layout for alphabet and numbers
        int charsPerRow = 10;
        int letterX = 20;
        int letterY = 110;
        int charWidth = 65;
        int charHeight = 45;
        int charIndex = 0;

        // Draw alphabet (A-Z)
        for (char c = 'A'; c <= 'Z'; c++) {
            auto it = keyboardMap.find(c);
            if (it != keyboardMap.end()) {
                int row = charIndex / charsPerRow;
                int col = charIndex % charsPerRow;
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + col * (charWidth - 15), letterY + row * (charHeight),
                    fontSize);
                charIndex++;
            }
        }

        // Reset for numbers
        letterY += (charIndex / charsPerRow + 1) * charHeight;
        charIndex = 0;

        // Draw numbers (0-9)
        DrawText("Numbers:", 20, letterY, 20, DARKGRAY);
        letterY += 25;

        for (char c = '0'; c <= '9'; c++) {
            auto it = keyboardMap.find(c);
            if (it != keyboardMap.end()) {
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + charIndex * (charWidth - 15), letterY,
                    fontSize);
                charIndex++;
            }
        }

        // Reset for symbols
        letterY += charHeight + 20;
        charIndex = 0;

        // Draw symbols
        DrawText("Symbols:", 20, letterY, 20, DARKGRAY);
        letterY += 25;

        // First row of symbols
        const char* symbols1 = "`-=[]\\;',./";
        for (int i = 0; symbols1[i] != '\0'; i++) {
            auto it = keyboardMap.find(symbols1[i]);
            if (it != keyboardMap.end()) {
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + charIndex * (charWidth - 20), letterY,
                    fontSize);
                charIndex++;
                if (charIndex == charsPerRow) {
                    letterY += charHeight;
                    charIndex = 0;
                }
            }
        }

        // Check if we need to move to a new row
        if (charIndex > 0) {
            letterY += charHeight;
            charIndex = 0;
        }

        // Draw special keys section
        letterY = 110;
        letterX = 550;
        charIndex = 0;

        DrawText("Special Keys:", letterX, 85, 20, DARKGRAY);

        // Function keys (F1-F12)
        for (int i = KEY_F1; i <= KEY_F12; i++) {
            auto it = keyboardMap.find(i);
            if (it != keyboardMap.end()) {
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + charIndex * (charWidth + 10), letterY,
                    fontSize);
                charIndex++;
                if (charIndex == 6) { // Break after F6
                    letterY += charHeight;
                    charIndex = 0;
                }
            }
        }

        // Navigation keys
        letterY += charHeight + 15;
        charIndex = 0;
        DrawText("Navigation:", letterX, letterY, 20, DARKGRAY);
        letterY += 30;

        const int navKeys[] = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE };
        for (int i = 0; i < 10; i++) {
            auto it = keyboardMap.find(navKeys[i]);
            if (it != keyboardMap.end()) {
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + charIndex * (charWidth + 10), letterY,
                    fontSize);
                charIndex++;
                if (charIndex == 5) { // Break after 5 keys
                    letterY += charHeight;
                    charIndex = 0;
                }
            }
        }

        // Modifier keys
        letterY += charHeight + 15;
        charIndex = 0;
        DrawText("Modifiers:", letterX, letterY, 20, DARKGRAY);
        letterY += 30;

        const int modKeys[] = { KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT, KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL, KEY_LEFT_ALT, KEY_RIGHT_ALT, KEY_LEFT_SUPER, KEY_RIGHT_SUPER };
        for (int i = 0; i < 8; i++) {
            auto it = keyboardMap.find(modKeys[i]);
            if (it != keyboardMap.end()) {
                DrawInputStatus(it->second.name, it->second.isPressed,
                    letterX + charIndex * (charWidth + 10), letterY,
                    fontSize);
                charIndex++;
                if (charIndex == 4) { // Break after 4 keys
                    letterY += charHeight;
                    charIndex = 0;
                }
            }
        }

        // Panel for mouse
        DrawRectangle(10, 630, otherPanelWidth, mousePanelHeight, Fade(LIGHTGRAY, 0.5f));
        DrawRectangleLines(10, 630, otherPanelWidth, mousePanelHeight, DARKGRAY);
        DrawText("MOUSE", 20, 640, 24, BLACK);

        // Draw mouse buttons status
        int mouseButtonX = 20;
        int mouseButtonY = 680;
        int mouseButtonSpacing = 25;

        for (const auto& button : mouseMap) {
            DrawInputStatus(button.second.name, button.second.isPressed,
                mouseButtonX, mouseButtonY, fontSize);
            mouseButtonY += mouseButtonSpacing;
        }

        // Mouse wheel
        DrawInputStatus("Wheel Up", mouseWheelUp, 220, 680, fontSize);
        DrawInputStatus("Wheel Down", mouseWheelDown, 220, 705, fontSize);

        // Panel for gamepad
        DrawRectangle(970, 40, otherPanelWidth, gamepadPanelHeight, Fade(LIGHTGRAY, 0.5f));
        DrawRectangleLines(970, 40, otherPanelWidth, gamepadPanelHeight, DARKGRAY);

        if (gamepadAvailable) {
            DrawText(TextFormat("GAMEPAD: %s", GetGamepadName(gamepadIndex)), 980, 50, 24, BLACK);

            // Draw gamepad buttons status
            int gamepadButtonX = 980;
            int gamepadButtonY = 90;

            for (const auto& button : gamepadMap) {
                DrawInputStatus(button.second.name, button.second.isPressed,
                    gamepadButtonX, gamepadButtonY, fontSize);
                gamepadButtonY += 20;

                // Create a second column after certain number of items
                if (gamepadButtonY > 250) {
                    gamepadButtonX = 1180;
                    gamepadButtonY = 90;
                }
            }

            // Draw analog sticks status
            float leftX = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_X);
            float leftY = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_LEFT_Y);
            float rightX = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_X);
            float rightY = GetGamepadAxisMovement(gamepadIndex, GAMEPAD_AXIS_RIGHT_Y);

            DrawText(TextFormat("Left Stick: (%.2f, %.2f)", leftX, leftY), 980, 270, fontSize, DARKGRAY);
            DrawText(TextFormat("Right Stick: (%.2f, %.2f)", rightX, rightY), 980, 300, fontSize, DARKGRAY);

            // Visual representation of sticks
            DrawCircle(1030, 370, 30, LIGHTGRAY);
            DrawCircle(1030 + leftX * 25, 370 + leftY * 25, 5, GREEN);

            DrawCircle(1130, 370, 30, LIGHTGRAY);
            DrawCircle(1130 + rightX * 25, 370 + rightY * 25, 5, GREEN);

            // Visual representation of Xbox controller (simple outline)
            int controllerX = 1050;
            int controllerY = 440;

            // Draw a simple controller outline
            DrawRectangle(controllerX - 50, controllerY, 100, 60, Fade(DARKGRAY, 0.2f));
            DrawRectangleLines(controllerX - 50, controllerY, 100, 60, DARKGRAY);

            // Draw buttons lit up when pressed
            // A button
            Color aColor = gamepadMap[GAMEPAD_BUTTON_RIGHT_FACE_DOWN].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 20, controllerY + 30, 5, aColor);

            // B button
            Color bColor = gamepadMap[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 30, controllerY + 20, 5, bColor);

            // X button
            Color xColor = gamepadMap[GAMEPAD_BUTTON_RIGHT_FACE_LEFT].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 10, controllerY + 20, 5, xColor);

            // Y button
            Color yColor = gamepadMap[GAMEPAD_BUTTON_RIGHT_FACE_UP].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 20, controllerY + 10, 5, yColor);

            // D-pad
            Color dpadUpColor = gamepadMap[GAMEPAD_BUTTON_LEFT_FACE_UP].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadRightColor = gamepadMap[GAMEPAD_BUTTON_LEFT_FACE_RIGHT].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadDownColor = gamepadMap[GAMEPAD_BUTTON_LEFT_FACE_DOWN].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadLeftColor = gamepadMap[GAMEPAD_BUTTON_LEFT_FACE_LEFT].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);

            DrawRectangle(controllerX - 30, controllerY + 20, 10, 5, dpadUpColor);
            DrawRectangle(controllerX - 15, controllerY + 35, 5, 10, dpadRightColor);
            DrawRectangle(controllerX - 30, controllerY + 45, 10, 5, dpadDownColor);
            DrawRectangle(controllerX - 45, controllerY + 35, 5, 10, dpadLeftColor);

            // Add TRIGGERS & SHOULDERS section
            DrawText("TRIGGERS & SHOULDERS", 980, 570, 24, BLACK);

            // Left Shoulder indicator
            Color lShoulderColor = gamepadMap[GAMEPAD_BUTTON_LEFT_TRIGGER_1].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawRectangle(1000, 620, 40, 20, lShoulderColor);
            DrawText("LB", 1010, 622, fontSize, BLACK);

            // Right Shoulder indicator
            Color rShoulderColor = gamepadMap[GAMEPAD_BUTTON_RIGHT_TRIGGER_1].isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawRectangle(1310, 620, 40, 20, rShoulderColor);
            DrawText("RB", 1320, 622, fontSize, BLACK);

            // Left Trigger gradient indicator
            DrawText("LT", 1000, 650, fontSize, BLACK);
            DrawRectangle(1000, 670, 150, 25, LIGHTGRAY);
            if (leftTrigger > 0) {
                DrawRectangle(1000, 670, (int)(leftTrigger * 150), 25, GREEN);
            }

            // Right Trigger gradient indicator
            DrawText("RT", 1325, 650, fontSize, BLACK);
            DrawRectangle(1200, 670, 150, 25, LIGHTGRAY);
            if (rightTrigger > 0) {
                DrawRectangle(1200 + (int)((1 - rightTrigger) * 150), 670, (int)(rightTrigger * 150), 25, GREEN);
            }
        }
        else {
            DrawText("NO GAMEPAD DETECTED", 980, 50, 24, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}