#pragma once

#include "Entity.h"
#include <vector>

namespace {
	static std::vector<Entity*> objectList;
	static std::vector<Entity*> selectedObjects;

	std::vector<Entity*> GetObjects() {
		return objectList;
	}

	std::vector<Entity*> GetSelectedObjects() {
		return selectedObjects;
	}

	Entity* GetObjectRecursive(char* name, std::vector<Entity*> list) {
		for (int i = 0; i < list.size(); i++) {
			if (list[i]->properties["Name"].Data == name)
				return list[i];
			else
				return GetObjectRecursive(name, list[i]->children);
		}

		return nullptr;
	}

	Entity* GetObject(char* name) {
		return GetObjectRecursive(name, objectList);
	}

	bool IsObjectSelected(char* name) {
		//return (GetObjectRecursive(name, selectedObjects) != nullptr);
		return (selectedObjects[0]->properties["Name"].Data == name);
	}

	void CleanupObjects() {
		for (int i = 0; i < GetObjects().size(); i++)
			delete objectList[i];

		objectList.clear();
	}

	void AddObject(Entity* object) {
		objectList.push_back(object);
	}

	void SelectObject(Entity* object) {
		selectedObjects.push_back(object);
	}

	void DeselectObjects() {
		selectedObjects.clear();
	}
}