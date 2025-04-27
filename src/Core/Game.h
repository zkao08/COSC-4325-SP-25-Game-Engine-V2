#pragma once

#include "Object.h"
#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>

class Game {
	private:
		std::vector<Object*> selectedObjects;
		std::unique_ptr<Object> gameObject;
	public:
		Game();
		~Game();

		Object* GetGameObject();
		std::vector<Object*> GetSelectedObjects();

		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(Object* object);
};