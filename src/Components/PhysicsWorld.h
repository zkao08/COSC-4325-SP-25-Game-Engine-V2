#pragma once
#include "box2d/box2d.h"
#include <vector>
#include <string>
#include <iostream>

enum class PhysicsBodyType {
	Static,
	Dynamic,
	Kinematic
};

enum class ShapeType {
    Box,
    Circle,
    Triangle,
    Capsule
};

struct PhysicsShapeParams{
    ShapeType shapeType = ShapeType::Box;
    PhysicsBodyType bodyType = PhysicsBodyType::Static;

    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f; // in degrees
    float linearDamping = 0.05f;
    float angularDamping = 1.0f;

    // Box, Triangle, Capsule
    float width = 1.0f;
    float height = 1.0f;

    // Circle, Capsule
    float radius = 0.5f;

    float friction = 0.3f;
    float density = 1.0f;
};

struct PlacedObject {
    b2BodyId bodyId;
    PhysicsShapeParams params;
    std::string shapeTag = "box"; // For editor filtering (optional)
    int editorId = -1;            // Optional unique ID
    bool selected = false;        // For selection in editor
};

class PhysicsWorld {
private:
	b2WorldId worldId;
    std::vector<PlacedObject> placedObjects;

    // Internal per-shape creation
    b2BodyId CreateBox(const PhysicsShapeParams& params);
    b2BodyId CreateCircle(const PhysicsShapeParams& params);
    b2BodyId CreateTriangle(const PhysicsShapeParams& params);
    b2BodyId CreateCapsule(const PhysicsShapeParams& params);
public:
	PhysicsWorld(float gravityX = 0.0f, float gravityY = -9.8f);
	~PhysicsWorld();

	void Step(float deltaTime, int stepCount = 4);
    b2BodyId CreateShape(const PhysicsShapeParams& params);
    const std::vector<PlacedObject>& GetPlacedObjects() const;
    void DestroyObject(b2BodyId id);
};