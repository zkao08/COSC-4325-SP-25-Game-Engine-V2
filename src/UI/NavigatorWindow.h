// NavigatorWindow UI Header
// Creates Navigator subwindow and adds functionality.

#pragma once

#include <iostream>
#include <vector>
#include "imgui.h"
#include "EntityWindow.h"
#include "Entity.h"
#include "Renderer.h"
#include "Game.h"
#include "Utility.h"

#include <memory>

class NavigatorWindow {
    public:
        static int Render(Renderer* renderer, float scale);
        static void GenerateItemTreeNodes(Entity* item);
        static void Cleanup();
        static void CreateTestItems();
};