// PropertiesWindow GUI
// Creates and renders the Properties subwindow.

#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "Game.h"
#include "Vector2.h"
#include "Utility.h"
#include "Object.h"

#include <stdexcept>
#include <string>

class PropertiesWindow {
    public:
        // Renders the GUI
        static int Render(Game* game, float scale);
        // Loads object property data into the GUI
        static void LoadProperties(Object* entity);
};