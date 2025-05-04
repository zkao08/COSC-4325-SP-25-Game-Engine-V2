// ViewportWindow UI Header
// Creates Viewport subwindow and adds functionality.

#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"
#include "Renderer.h"
#include "Camera.h"
#include "Vector3.h"
#include "Utility.h"
#include "Application.h"

#include <d3d11.h>

class ViewportWindow {
    private:
        static ImVec2 size;
        static ImVec2 position;
        static ImVec2 regionAvail;
    public:
        static int Render(Renderer* renderer, float scale, ImTextureID texture, Camera* camera);
        static ImVec2 GetSize();
        static bool IsFocused();
        static bool IsHovered();
};