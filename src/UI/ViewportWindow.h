// ViewportWindow UI Header
// Creates Viewport subwindow and adds functionality.

#pragma once

#include "imgui.h"
#include "Renderer.h"

#include <d3d11.h>

class ViewportWindow {
    private:
        static ImVec2 size;
    public:
        static int Render(Renderer* renderer, float scale, ImTextureID texture);
        static ImVec2 GetSize();
        static bool IsFocused();
};