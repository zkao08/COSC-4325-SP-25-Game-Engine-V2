#include "raylib.h"
#include "PhysicsWorld.h"
#include <iostream>
extern "C" b2Vec2 b2Body_GetPosition(b2BodyId body);
extern "C" b2Rot b2Body_GetRotation(b2BodyId body);

int main() {
    const float SCALE = 100.0f;         // 1 meter = 100 pixels
    const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
    float accumulator = 0.0f;
    double lastTime = GetTime();

    InitWindow(1000, 800, "Box2D + Raylib Physics Test");
    SetTargetFPS(60);

    PhysicsWorld world(0.0f, -4.0f);

    // --- Create ground (static box)
    PhysicsShapeParams groundParams;
    groundParams.shapeType = ShapeType::Box;
    groundParams.bodyType = PhysicsBodyType::Static;
    groundParams.x = 6.0f;
    groundParams.y = -1.0f;
    groundParams.width = 18.0f;
    groundParams.height = 2.0f;
    groundParams.friction = 0.5f;

    b2BodyId ground = world.CreateShape(groundParams);

    // --- Create a dynamic box
    PhysicsShapeParams boxParams;
    boxParams.shapeType = ShapeType::Box;
    boxParams.bodyType = PhysicsBodyType::Dynamic;
    boxParams.x = 4.0f;
    boxParams.y = 10.0f;
    boxParams.width = 1.0f;
    boxParams.height = 1.0f;
    boxParams.rotation = 0.0f;

    b2BodyId box = world.CreateShape(boxParams);

    // --- Create another rotated dynamic box
    PhysicsShapeParams box2Params = boxParams;
    box2Params.x = 4.5f;
    box2Params.y = 12.0f;
    box2Params.rotation = 30.0f;

    b2BodyId box2 = world.CreateShape(box2Params);

    // --- Create a dynamic circle
    PhysicsShapeParams circleParams;
    circleParams.shapeType = ShapeType::Circle;
    circleParams.bodyType = PhysicsBodyType::Dynamic;
    circleParams.x = 6.0f;
    circleParams.y = 13.0f;
    circleParams.radius = 0.5f; // meters
    circleParams.friction = 0.5f;
    
    b2BodyId circle = world.CreateShape(circleParams);

    // --- Create a dynamic triangle
    PhysicsShapeParams triParams;
    triParams.shapeType = ShapeType::Triangle;
    triParams.bodyType = PhysicsBodyType::Dynamic;
    triParams.x = 6.0f;
    triParams.y = 5.0f;
    triParams.width = 1.0f;
    triParams.height = 1.0f;
    triParams.friction = 0.5f;
    triParams.density = 1.0f;
    triParams.rotation = 45.0f;

    b2BodyId triangle = world.CreateShape(triParams);

    // --- Create a dynamic capsule
    PhysicsShapeParams capsuleParams;
    capsuleParams.shapeType = ShapeType::Capsule;
    capsuleParams.bodyType = PhysicsBodyType::Dynamic;
    capsuleParams.x = 4.0f;
    capsuleParams.y = 14.0f;
    capsuleParams.height = 2.0f;  // full capsule height including round ends
    capsuleParams.radius = 0.5f;  // radius of top and bottom
    capsuleParams.friction = 0.5f;
    capsuleParams.density = 1.0f;
    capsuleParams.rotation = 0.0f;

    b2BodyId capsule = world.CreateShape(capsuleParams);

    /*b2BodyId ground = world.CreateBox({ 4.0f, 1.0f, 8.0f, 2.0f, 0.0f, 0.5f, 1.0f, PhysicsBodyType::Static });

    b2BodyId ground = world.CreateBox({ 4.0f, 1.0f, 8.0f, 2.0f, 0.0f, 0.5f, 1.0f, PhysicsBodyType::Static });
    b2BodyId box = world.CreateBox({ 4.0f, 10.0f, 1.0f, 1.0f, 0.0f, 0.5f, 1.0f, PhysicsBodyType::Dynamic });
    b2BodyId box2 = world.CreateBox({ 4.5f, 12.0f, 1.0f, 1.0f, 45.0f, 0.5f, 1.0f, PhysicsBodyType::Dynamic });*/

    while (!WindowShouldClose()) {
        // --- Fixed Timestep Physics Simulation ---
        double now = GetTime();
        float frameTime = static_cast<float>(now - lastTime);
        lastTime = now;
        if (frameTime > 0.25f) frameTime = 0.25f;
        accumulator += frameTime;

        // --- Handle Input for box movement ---
        const float moveForce = 1.0f;
        if (IsKeyDown(KEY_RIGHT)) world.SetVelocity(box, moveForce);
        if (IsKeyDown(KEY_LEFT))  world.SetVelocity(box, -moveForce);
        /*if (IsKeyDown(KEY_UP))    world.ApplyForce(box, 0.0f, moveForce);
        if (IsKeyDown(KEY_DOWN))  world.ApplyForce(box, 0.0f, -moveForce);*/
        if (IsKeyDown(KEY_UP))    world.SetGravity(0.0f, 4.0f);
        if (IsKeyDown(KEY_DOWN))  world.SetGravity(0.0f, -4.0f);


        while (accumulator >= PHYSICS_TIMESTEP) {
            world.Step(PHYSICS_TIMESTEP);
            accumulator -= PHYSICS_TIMESTEP;
        }

        // --- Begin Drawing ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        /*float boxWidth = 1.0f * SCALE;
        float boxHeight = 1.0f * SCALE;
        float groundWidth = 8.0f * SCALE;
        float groundHeight = 2.0f * SCALE;*/

        // --- Get Physics Data ---
        b2Vec2 pos = b2Body_GetPosition(box);
        b2Rot rotation = b2Body_GetRotation(box);
        float rot = -atan2f(rotation.s, rotation.c) * RAD2DEG;


        b2Vec2 pos2 = b2Body_GetPosition(box2);
        b2Rot rotation2 = b2Body_GetRotation(box2);
        float rot2 = -atan2f(rotation2.s, rotation2.c) * RAD2DEG;

        //std::cout << "ROTATION OF box2 IN DEGREES: " << rot2 << std::endl;
        //std::cout << "POSITION OF box2: (" << pos2.x << ", " << pos2.y << ")" << std::endl;

        b2Vec2 groundPos = b2Body_GetPosition(ground);
        b2Vec2 posCircle = b2Body_GetPosition(circle);
        //std::cout << "POSITION OF circle: (" << posCircle.x << ", " << posCircle.y << ")" << std::endl;

        b2Vec2 posTri = b2Body_GetPosition(triangle);
        b2Rot rotTri = b2Body_GetRotation(triangle);
        float angleTri = atan2f(rotTri.s, rotTri.c);
        //std::cout << "ROTATION OF triangle IN DEGREES: " << angleTri * RAD2DEG << std::endl;
        //std::cout << "ROTATION OF box2 IN DEGREES: " << world.GetRotation(box2) << std::endl;

        // --- Calculations for Triangle ---
        // Triangle local vertices
        Vector2 triVerts[3] = {
            { -triParams.width * 0.5f, -triParams.height * 0.5f },
            {  triParams.width * 0.5f, -triParams.height * 0.5f },
            {  0.0f,                   triParams.height * 0.5f }
        };

        // Transform to world space
        for (int i = 0; i < 3; ++i) {
            float x = triVerts[i].x;
            float y = triVerts[i].y;

            // Rotate
            float rotatedX = x * cosf(angleTri) - y * sinf(angleTri);
            float rotatedY = x * sinf(angleTri) + y * cosf(angleTri);

            // Translate and scale
            triVerts[i].x = posTri.x * SCALE + rotatedX * SCALE;
            triVerts[i].y = 600 - (posTri.y * SCALE + rotatedY * SCALE);
        }

        // -------- DRAWING BEGINS HERE --------
        // --- Draw Ground ---
        DrawRectangle(
            static_cast<int>(groundPos.x * SCALE - (groundParams.width * SCALE) / 2),
            static_cast<int>(600 - groundPos.y * SCALE - (groundParams.height * SCALE) / 2),
            static_cast<int>(groundParams.width * SCALE),
            static_cast<int>(groundParams.height * SCALE),
            DARKGRAY
        );

        // --- Draw Box 1 (non-rotated) ---
        DrawRectanglePro(
            { pos.x * SCALE, 600 - pos.y * SCALE, boxParams.width * SCALE, boxParams.height * SCALE },
            { (boxParams.width * SCALE / 2), (boxParams.height * SCALE) / 2 },
            rot,
            BLUE
        );

        // --- Draw Box 2 (rotated) ---
        DrawRectanglePro(
            { pos2.x * SCALE, 600 - pos2.y * SCALE, box2Params.width * SCALE, box2Params.height * SCALE },
            { (box2Params.width * SCALE) / 2, (box2Params.height * SCALE) / 2 },
            rot2,
            BLUE
        );

        // --- Draw Circle ---
        DrawCircleV(
            { posCircle.x * SCALE, 600 - posCircle.y * SCALE },
            circleParams.radius * SCALE,
            RED
        );

        // --- Draw Triangle ---
        DrawTriangle(triVerts[0], triVerts[1], triVerts[2], GREEN);

        // --- Draw Capsule ---
        b2Vec2 posCap = b2Body_GetPosition(capsule);
        b2Rot rotCap = b2Body_GetRotation(capsule);
        float angle = -atan2f(rotCap.s, rotCap.c); // Raylib needs negative for Y-down

        float radius = capsuleParams.radius;
        float bodyH = capsuleParams.height - 2.0f * radius;
        float bodyW = radius * 2.0f;
        float halfBodyH = bodyH * 0.5f;
        float radiusPx = radius * SCALE;

        // Convert body position to screen center
        Vector2 center = { posCap.x * SCALE, 600 - posCap.y * SCALE };

        // --- Draw body (rectangle) ---
        DrawRectanglePro(
            { center.x, center.y, bodyW * SCALE, bodyH * SCALE },
            { (bodyW * SCALE) / 2, (bodyH * SCALE) / 2 },
            angle * RAD2DEG,
            ORANGE
        );

        // --- Circle offset (in world space)
        float dx = sinf(angle) * halfBodyH;
        float dy = cosf(angle) * halfBodyH;

        // --- Draw top and bottom caps
        DrawCircleV({ (posCap.x + dx) * SCALE, 600 - (posCap.y + dy) * SCALE }, radiusPx, ORANGE);
        DrawCircleV({ (posCap.x - dx) * SCALE, 600 - (posCap.y - dy) * SCALE }, radiusPx, ORANGE);


        //Debug info overlay:
        b2Vec2 shapePos = world.GetPosition(box2);
        float rotDeg = world.GetRotation(box2);
        b2Vec2 vel = world.GetVelocity(box2);
        b2Vec2 grav = world.GetGravity();

        DrawText(TextFormat("Pos:  %.2f, %.2f", shapePos.x, shapePos.y), 10, 10, 20, BLACK);
        DrawText(TextFormat("Rot:  %.1f", rotDeg), 10, 30, 20, BLACK);
        DrawText(TextFormat("Vel:  %.2f, %.2f", vel.x, vel.y), 10, 50, 20, BLACK);
        DrawText(TextFormat("Grav: %.2f, %.2f", grav.x, grav.y), 10, 70, 20, BLACK);


        EndDrawing();
        // -------- DRAWING ENDS HERE --------
    }

    CloseWindow();
    return 0;
}