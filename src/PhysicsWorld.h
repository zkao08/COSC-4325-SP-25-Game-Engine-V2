#pragma once
#include <box2d/box2d.h>

enum class PhysicsBodyType {
	Static,
	Dynamic,
	Kinematic
};

class PhysicsWorld {
private:
	b2WorldId worldId;
public:
	PhysicsWorld(float gravityX = 0.0f, float gravityY = -9.8f);
	~PhysicsWorld();

	void Step(float deltaTime, int stepCount = 4);
	b2BodyId CreateBox(float x = 10.0f, float y = 10.0f, float w = 2.0f, float h = 2.0f, PhysicsBodyType type = PhysicsBodyType::Static);
};