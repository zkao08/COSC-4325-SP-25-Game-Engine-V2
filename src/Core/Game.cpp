#include "Game.h"

Game::Game() {
	gameObject = std::make_unique<Object>("Game");
}

Game::~Game() {
	for (int i = 0; i < gameObject->children.size(); i++)
		delete gameObject->children[i];

	gameObject->children.clear();
}

Object* Game::GetGameObject() {
	return gameObject.get();
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