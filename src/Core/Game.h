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
		std::vector<Object*> selectedObjects;
		Object* gameObject;
		void SetEnabledRecursive(Object* object, bool runtime_state);
	public:
		bool enabled = true;
		bool devMode = false;

		Game(Object* game_object = nullptr, Renderer* renderer = nullptr, bool dev_mode = false);
		~Game();

		Object* GetGameObject();
		std::vector<Object*> GetSelectedObjects();

		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(Object* object);
		bool SetAllEnabled(bool runtime_state = false);
};