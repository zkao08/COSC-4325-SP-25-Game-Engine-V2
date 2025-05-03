#pragma once
#include <box2d/box2d.h>
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
	    PhysicsWorld();
	    ~PhysicsWorld();

        // Core system methods
        bool startUp(float gravityX, float gravityY);
        void shutDown();

        // Singleton accessor
        static PhysicsWorld& GetInstance();

        // Delete copy and move constructors/assignments
        PhysicsWorld(const PhysicsWorld&) = delete;
        PhysicsWorld& operator=(const PhysicsWorld&) = delete;
        PhysicsWorld(PhysicsWorld&&) = delete;
        PhysicsWorld& operator=(PhysicsWorld&&) = delete;

        // Getter Functions
        b2Vec2 GetPosition(b2BodyId id) const;
        float GetRotation(b2BodyId id) const;
        b2Vec2 GetVelocity(b2BodyId id) const;
        b2Vec2 GetGravity() const;

        // World Modifications
	    void Step(float deltaTime, int stepCount = 4);
        void SetGravity(float gravityX, float gravityY);

        // Shape creation and storing/deleting
        b2BodyId CreateShape(const PhysicsShapeParams& params);
        const std::vector<PlacedObject>& GetPlacedObjects() const;
        void DestroyObject(b2BodyId id);

        // Movement Control
        void ApplyForce(b2BodyId id, float forceX, float forceY);
        void SetVelocity(b2BodyId id, float velX);
};