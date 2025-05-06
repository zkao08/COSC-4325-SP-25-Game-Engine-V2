// ViewportWindow GUI
// Creates and renders the Viewport subwindow.

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

class Camera;

class ViewportWindow {
    private:
        static ImVec2 size;
        static ImVec2 position;
        static ImVec2 regionAvail;
    public:
        // Renders GUI
        static int Render(Renderer* renderer, float scale, ImTextureID texture, Camera* camera);
        // Gets the subwindow's size
        static ImVec2 GetSize();
        // Returns whether the viewport is focused by the user
        static bool IsFocused();
        // Returns whether viewport is hovered by the user
        static bool IsHovered();
};