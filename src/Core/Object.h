// Object Class
// Base class for all objects.

#pragma once

#define _USE_MATH_DEFINES

#include "PhysicsWorld.h"
#include "Renderer.h"
#include "Rect.h"
#include "Vector2.h"
#include "Utility.h"

#include <math.h>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

struct PropertyData {
	std::string DataType;
	std::string Data;
};

class Object {
	private:
		Object* DeleteChildRecursive(Object* object, std::vector<Object*> list, bool recursive = false);
		Object* GetChildRecursive(std::string name, std::vector<Object*> list, bool recursive = false);
		void CleanChildren();
		void CleanChildrenRecursive(Object* object);
		Object* IsDescendantRecursive(Object* object_to_find, Object* Object_to_search);
	public:
		Object* parent = nullptr;
		std::vector<Object*> children;
		std::unique_ptr<Rect> shape = nullptr;
		b2BodyId physicsBody;
		bool markedDeleted = false;

		std::map<std::string, PropertyData> properties{
			{"Name", {"char", "Object"}},
			{"Type", {"const_char", "Object"}},
			{"Parent", {"const_char", "null"}}
		};

		Object(std::string new_name);
		Object(Object* target_object, Renderer* renderer = nullptr, PhysicsWorld* physics_world = nullptr);
		Object(Renderer* renderer, std::string new_name, PhysicsWorld* physics_world = nullptr);
		Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData>, PhysicsWorld* physics_world = nullptr);
		~Object();

		void CreatePhysicsBody(PhysicsWorld* physics_world = nullptr, float scaleFactor = 1.0f);

		void AddChild(Object* child);
		void AddAfterChild(Object* child_target, Object* child);
		Object* GetChild(std::string name, bool recursive = false);
		void DeleteChild(Object* child, bool recursive = false);
		void RemoveChild(Object* child);

		bool IsDescendant(Object* object);

		void Update(bool dev_mode = false);
};