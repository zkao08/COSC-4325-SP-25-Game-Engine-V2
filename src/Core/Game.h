#pragma once

#include "Object.h"
#include <vector>
#include <string>

class Game {
	private:
		std::vector<Object*> objectList;
		std::vector<Object*> selectedObjects;

		Object* GetObjectRecursive(char* name, std::vector<Object*> list);
	public:
		~Game();
		void AddObject(Object* object);

		Object* GetObject(char* name);
		std::vector<Object*> GetObjects();
		std::vector<Object*> GetSelectedObjects();

		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(char* name);
};