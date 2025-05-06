// Game Class
// Handles the game world.

#pragma once

#include "PhysicsWorld.h"

#include "Renderer.h"
#include "Object.h"

#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>

class Object;

class Game {
	private:
		// List of objects selected in the level editor
		std::vector<Object*> selectedObjects;
		// Object that represents the game world
		Object* gameObject;
		// Recursive function to toggle enabled state for all objects in the game
		void SetEnabledRecursive(Object* object, bool runtime_state);
	public:
		// States
		bool enabled = true;
		bool devMode = false;

		// Constructor and Destructor
		Game(Object* game_object = nullptr, Renderer* renderer = nullptr, bool dev_mode = false);
		~Game();

		// Returns game world
		Object* GetGameObject();
		// Get currently selected objects in the Navigator
		std::vector<Object*> GetSelectedObjects();

		// Methods for object selection in the level editor
		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(Object* object);

		// Toggles game enabled state
		bool SetAllEnabled(bool runtime_state = false);
};