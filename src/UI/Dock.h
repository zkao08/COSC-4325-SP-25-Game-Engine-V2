// Dock Functional Header
// Enables and configures specified subwindows to be docked.

#pragma once

#include "imgui.h"
#include "imgui_internal.h"

static bool docksInitialized = false;

class Dock {
    public:
        static void Dock::SetDockingBehavior() {
            ImGuiID mainDockSpaceID = ImGui::DockSpaceOverViewport();

            if (!docksInitialized) {
                docksInitialized = true;

                ImGui::DockBuilderRemoveNode(mainDockSpaceID); // Resets window positions
                ImGui::DockBuilderAddNode(mainDockSpaceID);
                ImGui::DockBuilderSetNodeSize(mainDockSpaceID, ImGui::GetMainViewport()->Size);

                ImGuiID left = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Left, 0.25f, nullptr, &mainDockSpaceID);
                ImGuiID right = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Right, 0.75f, nullptr, &mainDockSpaceID);

                ImGui::DockBuilderDockWindow("Navigator", left);
                ImGui::DockBuilderDockWindow("Properties", left);
                ImGui::DockBuilderDockWindow("Viewport", right);

                ImGui::DockBuilderFinish(mainDockSpaceID);
            }
        }
};