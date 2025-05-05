#include "Object.h"

float LimitRotation(float rotation);

bool scriptRunning = false;

Object::Object(std::string new_name, bool dev_mode) {
	devMode = dev_mode;
	this->properties["Name"].Data = new_name;
	this->children = new std::vector<Object*>;
}

Object::Object(Object* target_object, Renderer* renderer, bool dev_mode) {
	devMode = dev_mode;
	this->properties = target_object->properties;
	this->children = new std::vector<Object*>;
	if (renderer != nullptr && this->properties.find("Texture") != this->properties.end()) {
		Vector2 position = StringToVector2(this->properties["Position"].Data);
		Vector2 size = StringToVector2(this->properties["Size"].Data);
		float rotation = std::stof(RoundString(this->properties["Position"].Data, 2));

		std::wstring wStr(this->properties["Texture"].Data.begin(), this->properties["Texture"].Data.end());
		this->shape = std::make_unique<Rect>(renderer);
		this->shape->Create(wStr, position.x, position.y, size.x, size.y, rotation);

		this->CreatePhysicsBody(renderer->GetScaleFactor());
	}
	for (int i = 0; i < target_object->children->size(); i++) {
		Object* child = new Object(target_object->children->at(i), renderer, dev_mode);
		this->AddChild(child);
	}
}

Object::Object(Renderer* renderer, std::string new_name, bool dev_mode) {
	devMode = dev_mode;
	this->properties["Name"].Data = new_name;
	this->children = new std::vector<Object*>;
	this->shape = std::make_unique<Rect>(renderer);
	this->shape->Create();

	this->CreatePhysicsBody(renderer->GetScaleFactor());
}

Object::Object(Renderer* renderer, std::string new_name, std::map<std::string, PropertyData> new_properties, bool dev_mode) {
	devMode = dev_mode;
	this->properties = new_properties;
	this->properties["Name"].Data = new_name;
	this->children = new std::vector<Object*>;

	if (this->properties.find("Texture") != this->properties.end()) {
		std::wstring wStr(this->properties["Texture"].Data.begin(), this->properties["Texture"].Data.end());
		this->shape = std::make_unique<Rect>(renderer);
		this->shape->Create(wStr);
	}

	this->CreatePhysicsBody(renderer->GetScaleFactor());
}

Object::~Object() {
	for (int i = 0; i < this->children->size(); i++) {
		if (!this->children->at(i)->markedDeleted)
			delete this->children->at(i);
	}

	delete children;
}

std::string Object::GetProperty(std::string property) {
	if (!enabled || markedDeleted || this->properties.find(property) == this->properties.end())
		return "";

	return this->properties[property].Data;
}

void Object::SetProperty(std::string property, std::string value) {
	if (!enabled || markedDeleted || this->properties.find(property) == this->properties.end())
		return;

	this->properties[property].Data = value;
}

void Object::Delete() {
	if (this->GetParent() == nullptr)
		delete this;
	else
		this->GetParent()->DeleteChild(this);
}

void Object::CreatePhysicsBody(float scaleFactor) {
	if (!enabled || devMode || markedDeleted || this->properties.find("Collidable") == this->properties.end() || this->properties["Collidable"].Data == "false")
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

	physicsBody = PhysicsWorld::GetInstance().CreateShape(physicsShapeParams);
}

Object* Object::GetParent() {
	return this->parent;
}

b2BodyId Object::GetPhysicsBodyId() {
	return physicsBody;
}

void Object::AddChild(Object* child) {
	if (!enabled || markedDeleted)
		return;
	child->parent = this;
	children->push_back(child);
}

void Object::AddAfterChild(Object* child_target, Object* child) {
	if (!enabled || markedDeleted)
		return;

	std::vector<Object*>* newChildren = new std::vector<Object*>;
	std::vector<Object*>* oldChildren = children;

	child->parent = this;

	for (int i = 0; i < children->size(); i++) {
		newChildren->push_back(children->at(i));
		if (children->at(i) == child_target)
			newChildren->push_back(child);
	}

	children = newChildren;
	delete oldChildren;
}

void Object::Update(bool dev_mode, Object* game, HWND hwnd) {
	if (!enabled || markedDeleted)
		return;

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
		if (this->shape.get() != nullptr) {
			if (b2Body_IsValid(physicsBody)) {
				b2Vec2 position = b2Body_GetPosition(this->physicsBody);
				b2Rot rotation = b2Body_GetRotation(this->physicsBody);
				float radians = atan2f(rotation.s, rotation.c) * (360 / M_PI) / 2;
				this->properties["Position"].Data = std::to_string(position.x) + "," + std::to_string(position.y);
				this->properties["Rotation"].Data = std::to_string(radians);
				if (this->properties["Static"].Data == "true" && b2Body_GetType(this->physicsBody) != b2BodyType::b2_staticBody)
					b2Body_SetType(this->physicsBody, b2BodyType::b2_staticBody);
				else if (b2Body_GetType(this->physicsBody) != b2BodyType::b2_dynamicBody)
					b2Body_SetType(this->physicsBody, b2BodyType::b2_dynamicBody);

				//if (this->properties["Upright"].Data == "true")
					//b2Body_SetAngularVelocity(this->physicsBody, 0.0f);

			}
			shape->SetTransform(StringToVector2(this->properties["Position"].Data), StringToVector2(this->properties["Size"].Data), std::stof(RoundString(this->properties["Rotation"].Data)));
			shape->LoadTexture(StringToWString(this->properties["Texture"].Data));
			shape->Render();
		}

		if (!ranScript) {
			std::thread scriptThread(&Object::ExecuteScript, this, this->properties["Script"].Data, game, hwnd);
			scriptThread.detach();
		}
	}

	for (int i = 0; i < children->size(); i++) {
		children->at(i)->Update(dev_mode, game, hwnd);
	}
}

Object* Object::GetChild(std::string name, bool recursive) {
	if (!enabled || markedDeleted)
		return nullptr;

	return GetChildRecursive(name, this->children, recursive);
}

Object* Object::GetChildRecursive(std::string name, std::vector<Object*>* list, bool recursive) {
	Object* obj = nullptr;

	for (int i = 0; i < list->size(); i++) {
		if (list->at(i)->properties["Name"].Data == name)
			return list->at(i);
		else if (recursive)
			obj = GetChildRecursive(name, list->at(i)->children, recursive);
		else
			return nullptr;
	}

	return obj;
}

void Object::DeleteChild(Object* object, bool recursive) {
	if (!enabled || markedDeleted)
		return;

	DeleteChildRecursive(object, this->children, recursive);
	CleanChildren();
}

Object* Object::DeleteChildRecursive(Object* object, std::vector<Object*>* list, bool recursive) {
	for (int i = 0; i < list->size(); i++) {
		if (!list->at(i)->markedDeleted && list->at(i) == object) {
			Object* obj = list->at(i);
			obj->markedDeleted = true;
			return nullptr;
		}
		else if (recursive)
			object = DeleteChildRecursive(object, list->at(i)->children, recursive);
		else
			return nullptr;
	}

	return object;
}

// Note that this is not the same as DeleteChild(). This removes it from the parent object, but does not delete it. Mainly used to transfer objects.
void Object::RemoveChild(Object* child) {
	if (!enabled || markedDeleted)
		return;

	for (int i = 0; i < children->size(); i++) {
		if (children->at(i) == child) {
			children->erase(children->begin() + i);
			break;
		}
	}

	child->parent = nullptr;
}

void Object::CleanChildren() {
	if (!enabled || markedDeleted)
		return;

	std::vector<Object*>* newList = new std::vector<Object*>;
	std::vector<Object*>* oldList = this->children;

	for (int i = 0; i < this->children->size(); i++) {
		if (this->children->at(i)->markedDeleted)
			delete this->children->at(i);
		else {
			CleanChildrenRecursive(this->children->at(i));
			newList->push_back(this->children->at(i));
		}
	}

	this->children = newList;
	delete oldList;
}

void Object::CleanChildrenRecursive(Object* object) {
	std::vector<Object*>* newList = new std::vector<Object*>;
	std::vector<Object*>* oldList = object->children;

	for (int i = 0; i < object->children->size(); i++) {
		if (object->children->at(i)->markedDeleted)
			delete object->children->at(i);
		else {
			CleanChildrenRecursive(object->children->at(i));
			newList->push_back(object->children->at(i));
		}
	}

	object->children = newList;
	delete oldList;
}

bool Object::IsDescendant(Object* object) {
	if (!enabled || markedDeleted)
		return false;

	return (IsDescendantRecursive(object, this) != nullptr);
}

Object* Object::IsDescendantRecursive(Object* object_to_find, Object* object_to_search) {
	Object* obj = nullptr;

	for (int i = 0; i < object_to_search->children->size(); i++) {
		if (object_to_search->children->at(i) == object_to_find)
			return object_to_find;
		else
			obj = IsDescendantRecursive(object_to_find, object_to_search->children->at(i));
	}

	return obj;
}

void Object::ExecuteScript(std::string file_path, Object* game, HWND hwnd) {
	if (!enabled || markedDeleted)
		return;

	sol::state lua;

	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::os);
	lua.open_libraries(sol::lib::coroutine);

	if (this->properties.find("Script") == this->properties.end() || this->properties["Script"].Data == "")
		return;

	if (ranScript)
		return;

	ranScript = true;

	if (file_path == "")
		file_path = this->properties["Script"].Data;

	lua.new_usertype<Object>("Object",
		"GetParent", &Object::GetParent,
		"GetProperty", &Object::GetProperty,
		"SetProperty", &Object::SetProperty,
		"GetPhysicsBodyId", &Object::GetPhysicsBodyId,
		"Delete", &Object::Delete,
		"enabled", &Object::enabled
	);

	lua.set("script", this);
	if (game != nullptr)
		lua.set("game", game);

	InputLuaAPI::Initialize(lua);
	AudioLuaAPI::Initialize(lua);
	PhysicsLuaAPI::Initialize(lua);

	try {
		lua.script_file(file_path);
		scriptRunning = true;
	}
	catch (const sol::error& e) {
		std::cerr << "Error loading Lua file in Object \"" << this->properties["Name"].Data << "\":" << e.what() << std::endl;
	}

	scriptRunning = false;
}

float LimitRotation(float rotation) {
	if (rotation > 360)
		rotation = LimitRotation(rotation - 360);
	else if (rotation < 0)
		rotation = LimitRotation(rotation + 360);

	return rotation;
}