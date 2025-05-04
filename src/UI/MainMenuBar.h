// MainMenuBar UI Header
// Creates top-level menu bar and adds functionality.

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