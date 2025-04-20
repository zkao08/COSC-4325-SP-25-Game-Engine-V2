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
        static int Render(Renderer* renderer, Game* game, float scale);
        static void GenerateItemTreeNodes(Entity* item, Game* game);
};