// Game Class
// Handles the game world.

// Note: The game world itself is considered an object.

#include "Game.h"

// Set or create game world
Game::Game(Object* game_object, Renderer* renderer, bool dev_mode) {
	devMode = dev_mode;

	if (game_object != nullptr)
		gameObject = new Object(game_object, renderer, dev_mode);
	else
		gameObject = new Object("Game", dev_mode);
}

// Destroy game world
Game::~Game() {
	delete gameObject;
}

// Returns game world
Object* Game::GetGameObject() {
	return gameObject;
}

// Get currently selected objects in the Navigator
std::vector<Object*> Game::GetSelectedObjects() {
	return selectedObjects;
}

// Selects an object in the Navigator
void Game::SelectObject(Object* object) {
	selectedObjects.push_back(object);
}

// Deselects an object in the navigator
void Game::DeselectObjects() {
	selectedObjects.clear();
}

// Return whether the provided object is selected
bool Game::IsObjectSelected(Object* object) {
	return (selectedObjects[0] == object);
}

// Toggle whether the game is enabled or not
bool Game::SetAllEnabled(bool enabled) {
	SetEnabledRecursive(gameObject, enabled);

	return 1;
}

// Recursive function to toggle enabled state for all objects in the game
void Game::SetEnabledRecursive(Object* object, bool enabled) {
	for (int i = 0; i < object->children->size(); i++)
		SetEnabledRecursive(object->children->at(i), enabled);

	object->enabled = enabled;
}