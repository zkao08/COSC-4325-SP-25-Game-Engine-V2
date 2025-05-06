// Object class
// Represents all types of objects in the game.

#pragma once

#define _USE_MATH_DEFINES

#include "GameEngine.h"
#include "PhysicsWorld.h"
#include "InputHandler.h"
#include "AudioLuaAPI.h"
#include "InputLuaAPI.h"
#include "PhysicsLuaAPI.h"
#include "Renderer.h"
#include "Rect.h"
#include "Vector2.h"
#include "Utility.h"
#include "Camera.h"

#include <sol/sol.hpp>
#include <math.h>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <map>

#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

struct PropertyData {
	std::string DataType;
	std::string Data;
};

class Camera;

class Object {
	private:
		// Object child methods
		Object* DeleteChildRecursive(Object* object, std::vector<Object*>* list, bool recursive = false);
		Object* GetChildRecursive(std::string name, std::vector<Object*>* list, bool recursive = false);
		void CleanChildren();
		void CleanChildrenRecursive(Object* object);
		Object* IsDescendantRecursive(Object* object_to_find, Object* Object_to_search);

		// Run Lua scripts
		void ExecuteScript(std::string file_path = "", Object* game = nullptr, HWND hwnd = nullptr, Camera* camera = nullptr);

		// Object physics body
		b2BodyId physicsBody;

		// States
		bool devMode = false;
		bool ranScript = false;
	public:
		// Objects
		Object* parent = nullptr;
		std::vector<Object*>* children;
		std::unique_ptr<Rect> shape = nullptr;

		// States
		bool enabled = true;
		bool markedDeleted = false;

		// Properties
		std::map<std::string, PropertyData> properties{
			{"Name", {"char", "Object"}},
			{"Type", {"const_char", "Object"}},
			{"Parent", {"const_char", "null"}}
		};

		// Constructors and Destructor
		Object(std::string new_name, bool dev_mode = false);
		Object(Object* target_object, Renderer* renderer = nullptr, bool dev_mode = false);
		Object(Renderer* renderer, std::string new_name, bool dev_mode = false);
		Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData>, bool dev_mode = false);
		~Object();

		// Property methods
		std::string GetProperty(std::string property);
		void SetProperty(std::string property, std::string value);

		// Object deletion
		void Delete();

		// Physics body creation
		void CreatePhysicsBody(float scaleFactor = 1.0f);

		// Object getters
		Object* GetParent();
		b2BodyId GetPhysicsBodyId();

		// Object child methods
		void AddChild(Object* child);
		void AddAfterChild(Object* child_target, Object* child);
		Object* GetChild(std::string name, bool recursive = false);
		void DeleteChild(Object* child, bool recursive = false);
		void RemoveChild(Object* child);

		// Checks whether provided object is a descendant
		bool IsDescendant(Object* object);

		// Get object positions
		float GetPositionX();
		float GetPositionY();

		// Updates object state
		void Update(bool dev_mode = false, Object* game = nullptr, HWND hwnd = nullptr, Camera* camera = nullptr);
};