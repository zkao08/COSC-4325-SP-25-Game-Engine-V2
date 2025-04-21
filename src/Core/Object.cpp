#include "Object.h"

#include <iostream>

Object::Object(Renderer* renderer, std::string new_name) {
	this->properties["Name"].Data = new_name;
	this->shape = std::make_unique<Rect>(renderer);
	this->shape->Create();
}

Object::Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData> new_properties) {
	this->properties = new_properties;
	this->properties["Name"].Data = new_name;
	if (this->properties.find("Position") != this->properties.end()) {
		this->shape = std::make_unique<Rect>(renderer);
		this->shape->Create();
	}
}

Object::~Object() {
	for (int i = 0; i < children.size(); i++)
		delete children[i];

	children.clear();
}

void Object::AddChild(Object* child) {
	child->parent = this;
	children.push_back(child);
}

void Object::Update() {
	if (this->shape.get() != nullptr) {
		shape->ChangePosition(StringToVector2(this->properties["Position"].Data));
		shape->Render();
	}
}