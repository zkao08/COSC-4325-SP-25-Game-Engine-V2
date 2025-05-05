// PhysicsLuaAPI.cpp
#include "PhysicsLuaAPI.h"
#include <iostream>

namespace PhysicsLuaAPI
{
    void Initialize(sol::state& lua)
    {
        // Get singleton instance
        PhysicsWorld& physicsWorld = PhysicsWorld::GetInstance();

        // Create the Physics API table
        sol::table physicsAPI = lua["Physics"].get_or_create<sol::table>();

        // Enums
        // BodyType
        sol::table bodyType = physicsAPI["BodyType"].get_or_create<sol::table>();
        bodyType["Static"] = static_cast<int>(PhysicsBodyType::Static);
        bodyType["Dynamic"] = static_cast<int>(PhysicsBodyType::Dynamic);
        bodyType["Kinematic"] = static_cast<int>(PhysicsBodyType::Kinematic);

        // ShapeType
        sol::table shapeType = physicsAPI["ShapeType"].get_or_create<sol::table>();
        shapeType["Box"] = static_cast<int>(ShapeType::Box);
        shapeType["Circle"] = static_cast<int>(ShapeType::Circle);
        shapeType["Triangle"] = static_cast<int>(ShapeType::Triangle);
        shapeType["Capsule"] = static_cast<int>(ShapeType::Capsule);

        // Register b2BodyId (opaque handle type)
        lua.new_usertype<b2BodyId>("BodyId",
            // Constructors - none, this is created by the physics engine
            sol::constructors<>()
            // No properties or methods, it's just a handle
        );

        // Register APIs that use singleton PhysicsWorld
        
        // Gravity
        physicsAPI["setGravity"] = [&physicsWorld](float x, float y) {
            physicsWorld.SetGravity(x, y);
        };
        
        physicsAPI["getGravity"] = [&physicsWorld, &lua]() -> sol::table {
            b2Vec2 gravity = physicsWorld.GetGravity();
            sol::table result = lua.create_table();
            result["x"] = gravity.x;
            result["y"] = gravity.y;
            return result;
        };
        
        // Body destruction
        physicsAPI["destroyBody"] = [&physicsWorld](b2BodyId bodyId) {
            physicsWorld.DestroyObject(bodyId);
        };
        
        // Body properties and manipulation
        physicsAPI["getPosition"] = [&physicsWorld, &lua](b2BodyId bodyId) -> sol::table {
            b2Vec2 pos = physicsWorld.GetPosition(bodyId);
            sol::table result = lua.create_table();
            result["x"] = pos.x;
            result["y"] = pos.y;
            return result;
        };
        
        physicsAPI["getRotation"] = [&physicsWorld](b2BodyId bodyId) -> float {
            return physicsWorld.GetRotation(bodyId);
        };
        
        physicsAPI["getVelocity"] = [&physicsWorld, &lua](b2BodyId bodyId) -> sol::table {
            b2Vec2 vel = physicsWorld.GetVelocity(bodyId);
            sol::table result = lua.create_table();
            result["x"] = vel.x;
            result["y"] = vel.y;
            return result;
        };
        
        physicsAPI["applyForce"] = [&physicsWorld](b2BodyId bodyId, float forceX, float forceY) {
            physicsWorld.ApplyForce(bodyId, forceX, forceY);
        };
        
        physicsAPI["setVelocity"] = [&physicsWorld](b2BodyId bodyId, float velX) {
            physicsWorld.SetVelocity(bodyId, velX);
        };
        
        std::cout << "Physics Lua API initialized successfully" << std::endl;
    }
}