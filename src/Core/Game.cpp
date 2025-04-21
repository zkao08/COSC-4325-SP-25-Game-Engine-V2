#include "Game.h"

Game::~Game() {
	for (int i = 0; i < objectList.size(); i++)
		delete objectList[i];

	objectList.clear();
}

void Game::AddObject(Object* object) {
	objectList.push_back(object);
}

Object* Game::GetObject(char* name) {
	return GetObjectRecursive(name, objectList);
}

Object* Game::GetObjectRecursive(char* name, std::vector<Object*> list) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i]->properties["Name"].Data == name)
			return list[i];
		else
			return GetObjectRecursive(name, list[i]->children);
	}

	return nullptr;
}

std::vector<Object*> Game::GetObjects() {
	return objectList;
}

std::vector<Object*> Game::GetSelectedObjects() {
	return selectedObjects;
}

void Game::SelectObject(Object* object) {
	selectedObjects.push_back(object);
}

void Game::DeselectObjects() {
	selectedObjects.clear();
}

bool Game::IsObjectSelected(char* name) {
	return (selectedObjects[0]->properties["Name"].Data == name);
}