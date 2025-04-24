#pragma once

#include "Object.h"
#include <vector>
#include <string>
#include <DirectXMath.h>

class Game {
	private:
		std::vector<Object*> objectList;
		std::vector<Object*> selectedObjects;

		Object* DeleteObjectRecursive(std::string name, std::vector<Object*> list);
		Object* GetObjectRecursive(std::string name, std::vector<Object*> list);
		void CleanObjectList();
		void CleanObjectListRecursive(Object* object);
	public:
		~Game();
		void AddObject(Object* object);
		void DeleteObject(std::string name);

		Object* GetObject(std::string name);
		std::vector<Object*> GetObjects();
		std::vector<Object*> GetSelectedObjects();

		void SelectObject(Object* object);
		void DeselectObjects();
		bool IsObjectSelected(Object* object);
};