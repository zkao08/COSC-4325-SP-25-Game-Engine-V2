// ObjectWindow GUI
// Creates and renders object creation popup subwindow.

#pragma once

#include "imgui.h"
#include "Object.h"
#include "Renderer.h"
#include "Game.h"

#include <memory>
#include <string>
#include <map>

class Game;

class ObjectWindow {
    public:
        static void Render(Renderer* renderer, Game* game, float scale);
        static void Toggle(bool toggle);
        static void Toggle();
};