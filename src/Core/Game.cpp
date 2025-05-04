#include "Game.h"

#include <iostream>

Game::Game(Object* game_object, Renderer* renderer, bool dev_mode) {
	devMode = dev_mode;
	physicsWorld = new PhysicsWorld();
	physicsWorld->startUp(0, -9.8);

	if (game_object != nullptr)
		gameObject = new Object(game_object, renderer, physicsWorld, dev_mode);
	else
		gameObject = new Object("Game", dev_mode);
}

Game::~Game() {
	delete gameObject;
	delete physicsWorld;
}

Object* Game::GetGameObject() {
	return gameObject;
}

PhysicsWorld* Game::GetPhysicsWorld() {
	return physicsWorld;
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

bool Game::SetAllEnabled(bool enabled) {
	SetEnabledRecursive(gameObject, enabled);

	return 1;
}

void Game::SetEnabledRecursive(Object* object, bool enabled) {
	for (int i = 0; i < object->children->size(); i++)
		SetEnabledRecursive(object->children->at(i), enabled);

	object->enabled = enabled;
}