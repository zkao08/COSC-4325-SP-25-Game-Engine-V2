#include "Game.h"

#include <iostream>

Game::Game(Object* game_object, Renderer* renderer) {
	physicsWorld = new PhysicsWorld();
	physicsWorld->startUp(0.0f, -9.8f);

	if (game_object != nullptr)
		gameObject = new Object(game_object, renderer, physicsWorld);
	else
		gameObject = new Object("Game");
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