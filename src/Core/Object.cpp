#include "Object.h"

float LimitRotation(float rotation);

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
	for (int i = 0; i < children.size(); i++) {
		if (!children[i]->markedDeleted)
			delete children[i];
	}

	children.clear();
}

void Object::AddChild(Object* child) {
	child->parent = this;
	children.push_back(child);
}

void Object::Update() {
	if (this->properties.find("Rotation") != this->properties.end()) {
		float num = std::stof(RoundString(this->properties["Rotation"].Data));
		num = LimitRotation(num);
		this->properties["Rotation"].Data = RoundString(std::to_string(num));
	}
	if (this->shape.get() != nullptr) {
		shape->SetTransform(StringToVector2(this->properties["Position"].Data), StringToVector2(this->properties["Size"].Data), std::stof(RoundString(this->properties["Rotation"].Data)));
		shape->Render();
	}
}

float LimitRotation(float rotation) {
	if (rotation > 360)
		rotation = LimitRotation(rotation - 360);
	else if (rotation < 0)
		rotation = LimitRotation(rotation + 360);

	return rotation;
}