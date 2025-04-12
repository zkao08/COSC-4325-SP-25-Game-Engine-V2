#include "InputHandler.h"

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

    // Create input handler
    InputHandler inputHandler;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update all inputs
        inputHandler.Update();

        // Get mouse position
        Vector2 mousePos = GetMousePosition();

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw mouse position
        DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", mousePos.x, mousePos.y), 10, 10, 24, DARKGRAY);

        // Draw shift status
        DrawText(TextFormat("Shift: %s", inputHandler.IsShiftPressed() ? "PRESSED" : "NOT PRESSED"), 400, 10, 24,
            inputHandler.IsShiftPressed() ? GREEN : DARKGRAY);

        // Draw GUI panels for different input devices
        int keyboardPanelWidth = 950;
        int keyboardPanelHeight = 580;
        int otherPanelWidth = 500;
        int gamepadPanelHeight = 700;
        int mousePanelHeight = 200;
        int fontSize = 22;

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

        // Get keyboard map
        const auto& keyboardMap = inputHandler.GetKeyboardMap();

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
                    letterX + charIndex * (charWidth + 8), letterY,
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
                    letterX + charIndex * (charWidth + 20), letterY,
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
                    letterX + charIndex * (charWidth + 35), letterY,
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

        // Get mouse map
        const auto& mouseMap = inputHandler.GetMouseMap();

        for (const auto& button : mouseMap) {
            DrawInputStatus(button.second.name, button.second.isPressed,
                mouseButtonX, mouseButtonY, fontSize);
            mouseButtonY += mouseButtonSpacing;
        }

        // Mouse wheel
        DrawInputStatus("Wheel Up", inputHandler.IsMouseWheelUp(), 220, 680, fontSize);
        DrawInputStatus("Wheel Down", inputHandler.IsMouseWheelDown(), 220, 705, fontSize);

        // Panel for gamepad
        DrawRectangle(970, 40, otherPanelWidth, gamepadPanelHeight, Fade(LIGHTGRAY, 0.5f));
        DrawRectangleLines(970, 40, otherPanelWidth, gamepadPanelHeight, DARKGRAY);

        if (inputHandler.IsGamepadAvailable()) {
            DrawText(TextFormat("GAMEPAD: %s", inputHandler.GetGamepadName()), 980, 50, 24, BLACK);

            // Draw gamepad buttons status
            int gamepadButtonX = 980;
            int gamepadButtonY = 90;

            // Get gamepad map
            const auto& gamepadMap = inputHandler.GetGamepadMap();

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
            float leftX = inputHandler.GetGamepadAxisLeftX();
            float leftY = inputHandler.GetGamepadAxisLeftY();
            float rightX = inputHandler.GetGamepadAxisRightX();
            float rightY = inputHandler.GetGamepadAxisRightY();

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
            Color aColor = gamepadMap.at(GAMEPAD_BUTTON_RIGHT_FACE_DOWN).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 20, controllerY + 30, 5, aColor);

            // B button
            Color bColor = gamepadMap.at(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 30, controllerY + 20, 5, bColor);

            // X button
            Color xColor = gamepadMap.at(GAMEPAD_BUTTON_RIGHT_FACE_LEFT).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 10, controllerY + 20, 5, xColor);

            // Y button
            Color yColor = gamepadMap.at(GAMEPAD_BUTTON_RIGHT_FACE_UP).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawCircle(controllerX + 20, controllerY + 10, 5, yColor);

            // D-pad
            Color dpadUpColor = gamepadMap.at(GAMEPAD_BUTTON_LEFT_FACE_UP).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadRightColor = gamepadMap.at(GAMEPAD_BUTTON_LEFT_FACE_RIGHT).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadDownColor = gamepadMap.at(GAMEPAD_BUTTON_LEFT_FACE_DOWN).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            Color dpadLeftColor = gamepadMap.at(GAMEPAD_BUTTON_LEFT_FACE_LEFT).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);

            DrawRectangle(controllerX - 30, controllerY + 20, 10, 5, dpadUpColor);
            DrawRectangle(controllerX - 15, controllerY + 35, 5, 10, dpadRightColor);
            DrawRectangle(controllerX - 30, controllerY + 45, 10, 5, dpadDownColor);
            DrawRectangle(controllerX - 45, controllerY + 35, 5, 10, dpadLeftColor);

            // Add TRIGGERS & SHOULDERS section
            DrawText("TRIGGERS & SHOULDERS", 980, 570, 24, BLACK);

            // Left Shoulder indicator
            Color lShoulderColor = gamepadMap.at(GAMEPAD_BUTTON_LEFT_TRIGGER_1).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawRectangle(1000, 620, 40, 20, lShoulderColor);
            DrawText("LB", 1010, 622, fontSize, BLACK);

            // Right Shoulder indicator
            Color rShoulderColor = gamepadMap.at(GAMEPAD_BUTTON_RIGHT_TRIGGER_1).isPressed ? GREEN : Fade(DARKGRAY, 0.5f);
            DrawRectangle(1310, 620, 40, 20, rShoulderColor);
            DrawText("RB", 1320, 622, fontSize, BLACK);

            // Left Trigger gradient indicator
            DrawText("LT", 1000, 650, fontSize, BLACK);
            DrawRectangle(1000, 670, 150, 25, LIGHTGRAY);
            if (inputHandler.GetLeftTrigger() > 0) {
                DrawRectangle(1000, 670, (int)(inputHandler.GetLeftTrigger() * 150), 25, GREEN);
            }

            // Right Trigger gradient indicator
            DrawText("RT", 1325, 650, fontSize, BLACK);
            DrawRectangle(1200, 670, 150, 25, LIGHTGRAY);
            if (inputHandler.GetRightTrigger() > 0) {
                DrawRectangle(1200 + (int)((1 - inputHandler.GetRightTrigger()) * 150), 670,
                    (int)(inputHandler.GetRightTrigger() * 150), 25, GREEN);
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