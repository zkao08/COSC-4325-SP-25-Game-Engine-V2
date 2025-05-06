// MainMenuBar GUI
// Creates and renders the top menu bar in the level editor.

#pragma once

#include "imgui.h"

enum MainMenuBarResult {
    NONE,
    CLOSE_APP,
    RUN_GAME
};

class MainMenuBar {
    public:
        static int Render();
};