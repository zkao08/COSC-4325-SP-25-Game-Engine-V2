// Object class
// Represents all types of objects in the game.

#include "Object.h"

float LimitRotation(float rotation);

bool scriptRunning = false;

// Creates a bare object. Mainly used for folders
Object::Object(std::string new_name, bool dev_mode) {
	devMode = dev_mode;
	this->properties["Name"].Data = new_name;
	this->children = new std::vector<Object*>;
}

// Creates a copy of the provided object
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

// Creates a bare object with a rendered shape
Object::Object(Renderer* renderer, std::string new_name, bool dev_mode) {
	devMode = dev_mode;
	this->properties["Name"].Data = new_name;
	this->children = new std::vector<Object*>;
	this->shape = std::make_unique<Rect>(renderer);
	this->shape->Create();

	this->CreatePhysicsBody(renderer->GetScaleFactor());
}

// Creates an object with set properties
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

// Destroys the object and its children
Object::~Object() {
	if (b2Body_IsValid(this->physicsBody))
		PhysicsWorld::GetInstance().DestroyObject(this->physicsBody);

	for (int i = 0; i < this->children->size(); i++) {
		if (!this->children->at(i)->markedDeleted)
			delete this->children->at(i);
	}

	delete children;
}

// Gets the property of an object
std::string Object::GetProperty(std::string property) {
	if (!enabled || markedDeleted || this->properties.find(property) == this->properties.end())
		return "";

	return this->properties[property].Data;
}

// Sets the property of an object
void Object::SetProperty(std::string property, std::string value) {
	if (!enabled || markedDeleted || this->properties.find(property) == this->properties.end())
		return;

	this->properties[property].Data = value;
}

// Deletes the object. Mainly used to bind to Lua.
void Object::Delete() {
	if (this->GetParent() == nullptr)
		delete this;
	else
		this->GetParent()->DeleteChild(this);
}

// Creates a body for the object to enable physics capabilities
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

// Gets the object's parent
Object* Object::GetParent() {
	return this->parent;
}

// Gets the object's physical body
b2BodyId Object::GetPhysicsBodyId() {
	return physicsBody;
}

// Adds a child object into the object
void Object::AddChild(Object* child) {
	if (!enabled || markedDeleted)
		return;
	child->parent = this;
	children->push_back(child);
}

// Adds a child object into the object after a provided target child object.
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

// Expected to be called in a loop to update the object's state
void Object::Update(bool dev_mode, Object* game, HWND hwnd, Camera* camera) {
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

			}
			shape->SetTransform(StringToVector2(this->properties["Position"].Data), StringToVector2(this->properties["Size"].Data), std::stof(RoundString(this->properties["Rotation"].Data)));
			shape->LoadTexture(StringToWString(this->properties["Texture"].Data));
			shape->Render();
		}

		if (!ranScript) {
			std::thread scriptThread(&Object::ExecuteScript, this, this->properties["Script"].Data, game, hwnd, camera);
			scriptThread.detach();
		}
	}

	for (int i = 0; i < children->size(); i++) {
		children->at(i)->Update(dev_mode, game, hwnd, camera);
	}
}

// Finds a child of the object by name if it exists
Object* Object::GetChild(std::string name, bool recursive) {
	if (!enabled || markedDeleted)
		return nullptr;

	return GetChildRecursive(name, this->children, recursive);
}

// Recursive function for GetChild()
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

// Deletes a child in the object
void Object::DeleteChild(Object* object, bool recursive) {
	if (!enabled || markedDeleted)
		return;

	DeleteChildRecursive(object, this->children, recursive);
	CleanChildren();
}

// Recursive function for DeleteChild()
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

// Unparents a child object
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

// Updates the object's children to delete objects marked as deleted
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

// Recursive function for CleanChildren()
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

// Returns whether the provided object is a descendant of the current object
bool Object::IsDescendant(Object* object) {
	if (!enabled || markedDeleted)
		return false;

	return (IsDescendantRecursive(object, this) != nullptr);
}

// Recursive function for IsDescendant()
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

// Runs Lua code and binds C++ methods for Lua to use
void Object::ExecuteScript(std::string file_path, Object* game, HWND hwnd, Camera* camera) {
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
		"GetPositionX", &Object::GetPositionX,
		"GetPositionY", &Object::GetPositionY,
		"Delete", &Object::Delete,
		"enabled", &Object::enabled
	);

	lua.new_usertype<Camera>("Camera",
		"Set", &Camera::Set,
		"FocusOnObject", &Camera::FocusOnObject
	);

	lua.set("script", this);
	if (camera != nullptr)
		lua.set("camera", camera);
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

// Gets the X position of the object. Mainly used to bind to Lua.
float Object::GetPositionX() {
	Vector2 position = StringToVector2(this->GetProperty("Position"));
	std::cout << -position.x << std::endl;
	return -position.x;
}

// Gets the Y position of the object. Mainly used to bind to Lua.
float Object::GetPositionY() {
	Vector2 position = StringToVector2(this->GetProperty("Position"));
	return -position.y;
}

// Limits a number to be within 0-360
float LimitRotation(float rotation) {
	if (rotation > 360)
		rotation = LimitRotation(rotation - 360);
	else if (rotation < 0)
		rotation = LimitRotation(rotation + 360);

	return rotation;
}