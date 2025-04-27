// NavigatorWindow UI Header
// Creates Navigator subwindow and adds functionality.

#pragma once

#include <iostream>
#include <vector>
#include "imgui.h"
#include "ObjectWindow.h"
#include "Object.h"
#include "Renderer.h"
#include "Game.h"
#include "Utility.h"

#include <memory>

class NavigatorWindow {
    private:
        static Object* heldObject;
    public:
        static int Render(Renderer* renderer, Game* game, float scale);
        static void GenerateItemTreeNodes(Object* item, Game* game, int id = 0);
};