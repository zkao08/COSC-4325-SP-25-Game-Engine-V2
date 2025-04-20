// EntityWindow UI Header
// Creates entity creation popup window and adds functionality.

#pragma once

#include "imgui.h"
#include "Classes.h"

class EntityWindow {
    public:
        static void RenderEntityWindow(float scale);
        static void ToggleEntityWindow(bool toggle);
        static void ToggleEntityWindow();
};