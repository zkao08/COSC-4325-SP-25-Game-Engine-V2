#include "Game.h"

Game::~Game() {
	for (int i = 0; i < objectList.size(); i++)
		delete objectList[i];

	objectList.clear();
}

void Game::AddObject(Object* object) {
	objectList.push_back(object);
	SelectObject(object);
}

void Game::DeleteObject(std::string name) {
	DeselectObjects();
	DeleteObjectRecursive(name, objectList);
	CleanObjectList();
}

Object* Game::DeleteObjectRecursive(std::string name, std::vector<Object*> list) {
	Object* obj = nullptr;

	for (int i = 0; i < list.size(); i++) {
		if (!list[i]->markedDeleted && list[i]->properties["Name"].Data == name) {
			Object* obj = list[i];
			obj->markedDeleted = true;
			return nullptr;
		}
		else
			obj = DeleteObjectRecursive(name, list[i]->children);
	}

	return obj;
}

void Game::CleanObjectList() {
	std::vector<Object*> newList;

	for (int i = 0; i < objectList.size(); i++) {
		if (objectList[i]->markedDeleted)
			delete objectList[i];
		else {
			CleanObjectListRecursive(objectList[i]);
			newList.push_back(objectList[i]);
		}
	}

	objectList = newList;
	newList.clear();
}

void Game::CleanObjectListRecursive(Object* object) {
	std::vector<Object*> newList;

	for (int i = 0; i < object->children.size(); i++) {
		if (object->children[i]->markedDeleted)
			delete object->children[i];
		else {
			CleanObjectListRecursive(object->children[i]);
			newList.push_back(object->children[i]);
		}
	}

	object->children = newList;
	newList.clear();
}

Object* Game::GetObject(std::string name) {
	return GetObjectRecursive(name, objectList);
}

Object* Game::GetObjectRecursive(std::string name, std::vector<Object*> list) {
	Object* obj = nullptr;

	for (int i = 0; i < list.size(); i++) {
		if (list[i]->properties["Name"].Data == name)
			return list[i];
		else
			obj = GetObjectRecursive(name, list[i]->children);
	}

	return obj;
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

bool Game::IsObjectSelected(Object* object) {
	return (selectedObjects[0] == object);
}