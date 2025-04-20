// PropertiesWindow UI Header
// Creates Properties subwindow and adds functionality.

#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "Game.h"
#include "Vector2.h"
#include "Utility.h"

#include <exception>
#include <string>

class PropertiesWindow {
    public:
        static int Render(float scale);
        static void LoadProperties(Entity* entity);
};