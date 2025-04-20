#pragma once

#include "Entity.h"
#include <vector>

class Game {
	public:
		static std::vector<Entity*> objectList;
		static std::vector<Entity*> selectedObjects;

		static std::vector<Entity*> GetObjects();
		static std::vector<Entity*> GetSelectedObjects();
		static Entity* GetObjectRecursive(char* name, std::vector<Entity*> list);
		static Entity* GetObject(char* name);

		static bool IsObjectSelected(char* name);
		static void CleanupObjects();
		static void AddObject(Entity* object);
		static void SelectObject(Entity* object);
		static void DeselectObjects();
};