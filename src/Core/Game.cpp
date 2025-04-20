#include "Game.h"

std::vector<Entity*> Game::GetObjects() {
	return objectList;
}

std::vector<Entity*> Game::GetSelectedObjects() {
	return selectedObjects;
}

Entity* Game::GetObjectRecursive(char* name, std::vector<Entity*> list) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i]->properties["Name"].Data == name)
			return list[i];
		else
			return GetObjectRecursive(name, list[i]->children);
	}

	return nullptr;
}

Entity* Game::GetObject(char* name) {
	return GetObjectRecursive(name, objectList);
}

bool Game::IsObjectSelected(char* name) {
	//return (GetObjectRecursive(name, selectedObjects) != nullptr);
	return (selectedObjects[0]->properties["Name"].Data == name);
}

void Game::CleanupObjects() {
	for (int i = 0; i < GetObjects().size(); i++)
		delete objectList[i];

	objectList.clear();
}

void Game::AddObject(Entity* object) {
	objectList.push_back(object);
}

void Game::SelectObject(Entity* object) {
	selectedObjects.push_back(object);
}

void Game::DeselectObjects() {
	selectedObjects.clear();
}