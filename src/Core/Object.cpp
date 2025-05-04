#include "Object.h"

float LimitRotation(float rotation);

Object::Object(std::string new_name) {
	this->properties["Name"].Data = new_name;
}

Object::Object(Object* target_object, Renderer* renderer, PhysicsWorld* physics_world) {
	this->properties = target_object->properties;
	if (renderer != nullptr && this->properties.find("Texture") != this->properties.end()) {
		Vector2 position = StringToVector2(this->properties["Position"].Data);
		Vector2 size = StringToVector2(this->properties["Size"].Data);
		float rotation = std::stof(RoundString(this->properties["Position"].Data, 2));

		std::wstring wStr(this->properties["Texture"].Data.begin(), this->properties["Texture"].Data.end());
		this->shape = std::make_unique<Rect>(renderer);
		this->shape->Create(wStr, position.x, position.y, size.x, size.y, rotation);
		
		this->CreatePhysicsBody(physics_world, renderer->GetScaleFactor());
	}
	for (int i = 0; i < target_object->children.size(); i++) {
		Object* child = new Object(target_object->children[i], renderer, physics_world);
		this->AddChild(child);
	}
}

Object::Object(Renderer* renderer, std::string new_name, PhysicsWorld* physics_world) {
	this->properties["Name"].Data = new_name;
	this->shape = std::make_unique<Rect>(renderer);
	this->shape->Create();

	this->CreatePhysicsBody(physics_world, renderer->GetScaleFactor());
}

Object::Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData> new_properties, PhysicsWorld* physics_world) {
	this->properties = new_properties;
	this->properties["Name"].Data = new_name;

	if (this->properties.find("Texture") != this->properties.end()) {
		std::wstring wStr(this->properties["Texture"].Data.begin(), this->properties["Texture"].Data.end());
		this->shape = std::make_unique<Rect>(renderer);
		this->shape->Create(wStr);
	}

	this->CreatePhysicsBody(physics_world, renderer->GetScaleFactor());
}

Object::~Object() {
	for (int i = 0; i < children.size(); i++) {
		if (!children[i]->markedDeleted)
			delete children[i];
	}

	children.clear();
}

void Object::CreatePhysicsBody(PhysicsWorld* physics_world, float scaleFactor) {
	if (physics_world == nullptr) {
		std::cout << "Cannot create physics body as the physics world is null." << std::endl;
		return;
	}
	else if (this->properties["Collidable"].Data == "false")
		return;

	Vector2 position = StringToVector2(this->properties["Position"].Data);
	Vector2 size = StringToVector2(this->properties["Size"].Data);
	float rotation = std::stof(RoundString(this->properties["Rotation"].Data, 2));

	PhysicsShapeParams physicsShapeParams;
	physicsShapeParams.shapeType = ShapeType::Box;
	physicsShapeParams.bodyType = this->properties["Static"].Data == "false" ? PhysicsBodyType::Dynamic : PhysicsBodyType::Static;
	physicsShapeParams.x = position.x;
	physicsShapeParams.y = position.y;
	physicsShapeParams.rotation = rotation;
	physicsShapeParams.width = size.x * scaleFactor;
	physicsShapeParams.height = size.y * scaleFactor;
	physicsShapeParams.friction = 0.5f;

	physicsBody = physics_world->CreateShape(physicsShapeParams);
}

void Object::AddChild(Object* child) {
	child->parent = this;
	children.push_back(child);
}

void Object::AddAfterChild(Object* child_target, Object* child) {
	std::vector<Object*> newChildren;

	child->parent = this;

	for (int i = 0; i < children.size(); i++) {
		newChildren.push_back(children[i]);
		if (children[i] == child_target)
			newChildren.push_back(child);
	}

	children = newChildren;
}

void Object::Update(bool dev_mode) {
	if (this->properties.find("Parent") != this->properties.end() && this->parent != nullptr) {
		this->properties["Parent"].Data = this->parent->properties["Name"].Data;
	}
	if (dev_mode) {
		if (this->properties.find("Rotation") != this->properties.end()) {
			float num = std::stof(RoundString(this->properties["Rotation"].Data));
			num = LimitRotation(num);
			this->properties["Rotation"].Data = RoundString(std::to_string(num));
		}
		if (this->shape.get() != nullptr) {
			shape->SetTransform(StringToVector2(this->properties["Position"].Data), StringToVector2(this->properties["Size"].Data), std::stof(RoundString(this->properties["Rotation"].Data)));
			shape->LoadTexture(StringToWString(this->properties["Texture"].Data));
			shape->Render();
		}
	}
	else {
		/*if (this->properties.find("Rotation") != this->properties.end()) {
			b2Rot rotation = b2Body_GetRotation(this->physicsBody);
			float radians = atan2f(rotation.s, rotation.c) * (M_PI / 180);
			this->properties["Rotation"].Data = std::to_string(radians);
		}*/
		if (this->shape.get() != nullptr) {
			b2Vec2 position = b2Body_GetPosition(this->physicsBody);
			b2Rot rotation = b2Body_GetRotation(this->physicsBody);
			float radians = atan2f(rotation.s, rotation.c) * (360 / M_PI) / 2;
			this->properties["Position"].Data = std::to_string(position.x) + "," + std::to_string(position.y);
			this->properties["Rotation"].Data = std::to_string(radians);
			shape->SetTransform(StringToVector2(this->properties["Position"].Data), StringToVector2(this->properties["Size"].Data), std::stof(RoundString(this->properties["Rotation"].Data)));
			shape->LoadTexture(StringToWString(this->properties["Texture"].Data));
			shape->Render();
		}
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->Update(dev_mode);
	}
}

Object* Object::GetChild(std::string name, bool recursive) {
	return GetChildRecursive(name, this->children, recursive);
}

Object* Object::GetChildRecursive(std::string name, std::vector<Object*> list, bool recursive) {
	Object* obj = nullptr;

	for (int i = 0; i < list.size(); i++) {
		if (list[i]->properties["Name"].Data == name)
			return list[i];
		else if (recursive)
			obj = GetChildRecursive(name, list[i]->children, recursive);
		else
			return nullptr;
	}

	return obj;
}

void Object::DeleteChild(Object* object, bool recursive) {
	DeleteChildRecursive(object, this->children, recursive);
	CleanChildren();
}

Object* Object::DeleteChildRecursive(Object* object, std::vector<Object*> list, bool recursive) {
	for (int i = 0; i < list.size(); i++) {
		if (!list[i]->markedDeleted && list[i] == object) {
			Object* obj = list[i];
			obj->markedDeleted = true;
			return nullptr;
		}
		else if (recursive)
			object = DeleteChildRecursive(object, list[i]->children, recursive);
		else
			return nullptr;
	}

	return object;
}

// Note that this is not the same as DeleteChild(). This removes it from the parent object, but does not delete it. Mainly used to transfer objects.
void Object::RemoveChild(Object* child) {
	for (int i = 0; i < children.size(); i++) {
		if (children[i] == child) {
			children.erase(children.begin() + i);
			break;
		}
	}

	child->parent = nullptr;
}

void Object::CleanChildren() {
	std::vector<Object*> newList;

	for (int i = 0; i < this->children.size(); i++) {
		if (this->children[i]->markedDeleted)
			delete this->children[i];
		else {
			CleanChildrenRecursive(this->children[i]);
			newList.push_back(this->children[i]);
		}
	}

	this->children = newList;
	newList.clear();
}

void Object::CleanChildrenRecursive(Object* object) {
	std::vector<Object*> newList;

	for (int i = 0; i < object->children.size(); i++) {
		if (object->children[i]->markedDeleted)
			delete object->children[i];
		else {
			CleanChildrenRecursive(object->children[i]);
			newList.push_back(object->children[i]);
		}
	}

	object->children = newList;
	newList.clear();
}

bool Object::IsDescendant(Object* object) {
	return (IsDescendantRecursive(object, this) != nullptr);
}

Object* Object::IsDescendantRecursive(Object* object_to_find, Object* object_to_search) {
	Object* obj = nullptr;

	for (int i = 0; i < object_to_search->children.size(); i++) {
		if (object_to_search->children[i] == object_to_find)
			return object_to_find;
		else
			obj = IsDescendantRecursive(object_to_find, object_to_search->children[i]);
	}

	return obj;
}

float LimitRotation(float rotation) {
	if (rotation > 360)
		rotation = LimitRotation(rotation - 360);
	else if (rotation < 0)
		rotation = LimitRotation(rotation + 360);

	return rotation;
}