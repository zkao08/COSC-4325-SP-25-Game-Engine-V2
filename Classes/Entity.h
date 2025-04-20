// Entity Class
// Top-level base class for all objects.
// Note: This class can be created. Common use is to act as a folder.

#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <Vector2.h>

struct PropertyData {
	std::string DataType;
	std::string Data;
};

class Entity {
	public:
		Entity* parent = nullptr;
		std::vector<Entity*> children;

		std::map<std::string, PropertyData> properties {
			{"Name", {"char", "Entity"}},
			{"Position", {"Vector2", "0,0"}},
			{"Rotation", {"float", "0.0"}}
		};

		Entity() {}

		Entity(char* new_name) {
			properties["Name"].Data = new_name;
		}

		Entity(char* new_name, Entity* new_parent) {
			properties["Name"].Data = new_name;
			parent = new_parent;
		}

		~Entity() {
			for (int i = 0; i < children.size(); i++)
				delete children[i];

			children.clear();
		}

		void AddChild(Entity* child) {
			child->parent = this;
			children.push_back(child);
		}
};