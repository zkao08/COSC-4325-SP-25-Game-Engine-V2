#include "PhysicsWorld.h"

// Operator overloading == for DestroyObject() function
inline bool operator==(const b2BodyId& a, const b2BodyId& b) {
	return a.index1 == b.index1 && a.generation == b.generation;
}

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

const std::vector<PlacedObject>& PhysicsWorld::GetPlacedObjects() const {
	return placedObjects;
}

void PhysicsWorld::DestroyObject(b2BodyId id) {
	b2DestroyBody(id);

	placedObjects.erase(
		std::remove_if(placedObjects.begin(), placedObjects.end(),
			[&](const PlacedObject& obj) {
				return obj.bodyId == id;
			}),
		placedObjects.end()
	);
}

b2BodyId PhysicsWorld::CreateShape(const PhysicsShapeParams& params) {
	b2BodyId body;

	switch (params.shapeType) {
		case ShapeType::Box:
			body = CreateBox(params);
			break;
		case ShapeType::Circle:
			body = CreateCircle(params);
			break;
		case ShapeType::Triangle:
			body = CreateTriangle(params);
			break;
		case ShapeType::Capsule:
			body = CreateCapsule(params);
			break;
		default:
			return {}; // Invalid
	}

	placedObjects.push_back({
		body,
		params,
		"shape",   // optional: change this per shape type
		static_cast<int>(placedObjects.size()) // unique editor ID
		});

	return body;
}

b2BodyId PhysicsWorld::CreateBox(const PhysicsShapeParams& params) {
	b2BodyDef bodyDef = b2DefaultBodyDef(); // creating base default body definition

	// Setting body type
	switch (params.bodyType) {
		case PhysicsBodyType::Static:
			bodyDef.type = b2_staticBody;
			break;
		case PhysicsBodyType::Kinematic:
			bodyDef.type = b2_kinematicBody;
			break;
		case PhysicsBodyType::Dynamic:
			bodyDef.type = b2_dynamicBody;
			break;
	}

	// Setting body rotation and position
	bodyDef.position = { params.x, params.y };
	bodyDef.rotation = b2MakeRot(params.rotation * (B2_PI/180.0f)); // converting to radians
	bodyDef.angularDamping = params.angularDamping;
	bodyDef.linearDamping = params.linearDamping;

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon box = b2MakeBox(params.width * 0.5f, params.height * 0.5f); // creating box polygon
	b2ShapeDef shapeDef = b2DefaultShapeDef(); // creating the shape (box)
	shapeDef.friction = params.friction;
	shapeDef.density = (params.bodyType == PhysicsBodyType::Dynamic) ? params.density : 0.0f;

	b2CreatePolygonShape(bodyId, &shapeDef, &box); // creating polygon shape and attaching the body
	return bodyId;
}

b2BodyId PhysicsWorld::CreateCircle(const PhysicsShapeParams& params) {
	b2BodyDef bodyDef = b2DefaultBodyDef();

	switch (params.bodyType) {
		case PhysicsBodyType::Static:
			bodyDef.type = b2_staticBody;
			break;
		case PhysicsBodyType::Kinematic:
			bodyDef.type = b2_kinematicBody;
			break;
		case PhysicsBodyType::Dynamic:
			bodyDef.type = b2_dynamicBody;
			break;
	}

	bodyDef.position = { params.x, params.y };
	bodyDef.rotation = b2MakeRot(params.rotation * (B2_PI / 180.0f));
	bodyDef.angularDamping = params.angularDamping;
	bodyDef.linearDamping = params.linearDamping;

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2Circle circle = { circle.center = {0.0f, 0.0f}, params.radius };
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.friction = params.friction;
	shapeDef.density = (params.bodyType == PhysicsBodyType::Dynamic) ? params.density : 0.0f;

	b2CreateCircleShape(bodyId, &shapeDef, &circle);
	return bodyId;
}

b2BodyId PhysicsWorld::CreateTriangle(const PhysicsShapeParams& params) {
	b2BodyDef bodyDef = b2DefaultBodyDef();
	
	switch (params.bodyType) {
		case PhysicsBodyType::Static:
			bodyDef.type = b2_staticBody;
			break;
		case PhysicsBodyType::Kinematic:
			bodyDef.type = b2_kinematicBody;
			break;
		case PhysicsBodyType::Dynamic:
			bodyDef.type = b2_dynamicBody;
			break;
	}
	bodyDef.position = { params.x, params.y };
	bodyDef.rotation = b2MakeRot(params.rotation * (B2_PI / 180.0f));
	bodyDef.angularDamping = params.angularDamping;   // slows down spinning over time
	bodyDef.linearDamping = params.linearDamping;

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	// Define triangle points
	const int VERTS = 3; // number of verticies (3 for a triangle)
	b2Vec2 vertices[VERTS] = {
		{ -params.width * 0.5f, -params.height * 0.5f },
		{  params.width * 0.5f, -params.height * 0.5f },
		{  0.0f,                 params.height * 0.5f }
	};

	// Compute convex hull
	b2Hull hull = b2ComputeHull(vertices, VERTS);
	// hull check if failed
	if (hull.count < 3) {
		std::cerr << "Failed to create valid triangle hull." << std::endl;
		return {};
	}

	float radius = 0.01f; // for polygon radius (collision checking)
	b2Polygon triangle = b2MakePolygon(&hull, radius);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.friction = params.friction;
	shapeDef.density = (params.bodyType == PhysicsBodyType::Dynamic) ? params.density : 0.0f;

	b2CreatePolygonShape(bodyId, &shapeDef, &triangle);
	return bodyId;
}

b2BodyId PhysicsWorld::CreateCapsule(const PhysicsShapeParams& params) {
	b2BodyDef bodyDef = b2DefaultBodyDef();

	switch (params.bodyType) {
		case PhysicsBodyType::Static:
			bodyDef.type = b2_staticBody;
			break;
		case PhysicsBodyType::Kinematic:
			bodyDef.type = b2_kinematicBody;
			break;
		case PhysicsBodyType::Dynamic:
			bodyDef.type = b2_dynamicBody;
			break;
	}

	bodyDef.position = { params.x, params.y };
	bodyDef.rotation = b2MakeRot(params.rotation * (B2_PI / 180.0f));
	bodyDef.angularDamping = 1.0f;
	bodyDef.linearDamping = 0.05f;

	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	// Capsule dimensions
	float capsuleRadius = params.radius;
	float capsuleHeight = params.height - 2.0f * capsuleRadius;
	if (capsuleHeight < 0.0f) capsuleHeight = 0.0f; // clamp

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.friction = params.friction;
	shapeDef.density = (params.bodyType == PhysicsBodyType::Dynamic) ? params.density : 0.0f;

	// Center box
	if (capsuleHeight > 0.0f) {
		b2Polygon box = b2MakeBox(params.radius, capsuleHeight * 0.5f);
		b2CreatePolygonShape(bodyId, &shapeDef, &box);
	}

	// Top circle
	b2Circle topCircle = {};
	topCircle.center = { 0.0f, capsuleHeight * 0.5f };
	topCircle.radius = capsuleRadius;
	b2CreateCircleShape(bodyId, &shapeDef, &topCircle);

	// Bottom circle
	b2Circle bottomCircle = {};
	bottomCircle.center = { 0.0f, -capsuleHeight * 0.5f };
	bottomCircle.radius = capsuleRadius;
	b2CreateCircleShape(bodyId, &shapeDef, &bottomCircle);

	return bodyId;
}