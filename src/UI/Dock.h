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

                ImGui::DockBuilderRemoveNode(mainDockSpaceID); // Resets window positions. Call this only once (ever) if you want window positions saved.
                ImGui::DockBuilderAddNode(mainDockSpaceID);
                ImGui::DockBuilderSetNodeSize(mainDockSpaceID, ImGui::GetMainViewport()->Size);

                ImGuiID left = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Left, 0.25f, nullptr, &mainDockSpaceID);
                ImGuiID right = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Right, 0.75f, nullptr, &mainDockSpaceID);
                //ImGuiID up = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Up, 0.1f, nullptr, &mainDockSpaceID);
                //ImGuiID down = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Down, 0.4f, nullptr, &mainDockSpaceID);
                //ImGuiID center = ImGui::DockBuilderSplitNode(mainDockSpaceID, ImGuiDir_Left, 0.4f, &left, &right);

                ImGui::DockBuilderDockWindow("Navigator", left);
                ImGui::DockBuilderDockWindow("Properties", left);
                //ImGui::DockBuilderDockWindow("Home", up);
                //ImGui::DockBuilderDockWindow("Edit", up);
                ImGui::DockBuilderDockWindow("Viewport", right);

                ImGui::DockBuilderFinish(mainDockSpaceID);
            }
        }
};