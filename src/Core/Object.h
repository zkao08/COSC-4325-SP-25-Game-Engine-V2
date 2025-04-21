// Object Class
// Base class for all objects.

#pragma once

#include "Renderer.h"
#include "Rect.h"
#include "Vector2.h"
#include "Utility.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

struct PropertyData {
	std::string DataType;
	std::string Data;
};

class Object {
	public:
		Object* parent = nullptr;
		std::vector<Object*> children;
		std::unique_ptr<Rect> shape = nullptr;

		std::map<std::string, PropertyData> properties {
			{"Name", {"char", "Object"}},
			{"Position", {"Vector2", "0,0"}},
			{"Rotation", {"float", "0.0"}},
			{"Size", {"Vector2", "0,0"}}
		};

		Object(Renderer* renderer, std::string new_name);

		Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData>);

		~Object();

		void AddChild(Object* child);

		void Update();
};