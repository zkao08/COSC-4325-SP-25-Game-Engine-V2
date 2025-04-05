#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld(float gravityX, float gravityY) {
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { gravityX, gravityY };
	worldId = b2CreateWorld(&worldDef);
}

PhysicsWorld::~PhysicsWorld() {
	b2DestroyWorld(worldId);
}

void PhysicsWorld::Step(float deltaTime, int stepCount) {
	b2World_Step(worldId, deltaTime, stepCount);
}

b2BodyId PhysicsWorld::CreateBox(float x, float y, float w, float h, PhysicsBodyType type) {
	b2BodyDef bodyDef = b2DefaultBodyDef();

	switch (type) {
	case PhysicsBodyType::Static:
		bodyDef.type = b2_staticBody;
		break;
	case PhysicsBodyType::Kinematic:
		bodyDef.type = b2_kinematicBody;
		break;
	case PhysicsBodyType::Dynamic:
		bodyDef.type = b2_dynamicBody;
		break;
	default:
		bodyDef.type = b2_staticBody;
		break;
	}

	bodyDef.position = { x, y };
	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon box = b2MakeBox(w * 0.5f, h * 0.5f);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.friction = 0.3f;
	shapeDef.density = (type == PhysicsBodyType::Dynamic) ? 1.0f : 0.0f;

	b2CreatePolygonShape(bodyId, &shapeDef, &box);
	return bodyId;
}