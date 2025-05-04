#pragma once

#include "PhysicsWorld.h"

#include "Renderer.h"
#include "Object.h"

#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>

class Game {
	private:
		std::vector<Object*> selectedObjects;
		Object* gameObject;
		PhysicsWorld* physicsWorld;
	public:
		Game(Object* game_object = nullptr, Renderer* renderer = nullptr);
		~Game();

		Object* GetGameObject();
		PhysicsWorld* GetPhysicsWorld();
		std::vector<Object*> GetSelectedObjects();

		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(Object* object);
};