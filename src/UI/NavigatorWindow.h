// NavigatorWindow GUI
// Creates and renders Navigator subwindow.

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

class Game;
class Object;

class NavigatorWindow {
    private:
        // Object being held. Used when moving objects around the Navigator.
        static Object* heldObject;
    public:
        // Render window
        static int Render(Renderer* renderer, Game* game, float scale);
        // Render object buttons
        static void GenerateItemTreeNodes(Object* item, Game* game, int id = 0);
};