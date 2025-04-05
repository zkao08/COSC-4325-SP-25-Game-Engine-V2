#include "raylib.h"
#include "PhysicsWorld.h"
extern "C" b2Vec2 b2Body_GetPosition(b2BodyId body);

int main() {
    const float SCALE = 100.0f;         // 1 meter = 100 pixels
    const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
    float accumulator = 0.0f;
    double lastTime = GetTime();

    InitWindow(800, 600, "Box2D + Raylib Physics Test");
    SetTargetFPS(300);  // Can be anything now

    PhysicsWorld world(0.0f, -4.0f);
    b2BodyId ground = world.CreateBox(4.0f, 1.0f, 8.0f, 2.0f, PhysicsBodyType::Static);
    b2BodyId box = world.CreateBox(4.0f, 10.0f, 1.0f, 1.0f, PhysicsBodyType::Dynamic);
    b2BodyId box2 = world.CreateBox(5.0f, 25.0f, 1.0f, 1.0f, PhysicsBodyType::Dynamic);

    while (!WindowShouldClose()) {
        // --- Fixed Timestep Physics Simulation ---
        double now = GetTime();
        float frameTime = static_cast<float>(now - lastTime);
        lastTime = now;

        // Clamp to avoid spiral of death (e.g. if paused or debugger attached)
        if (frameTime > 0.25f) frameTime = 0.25f;

        accumulator += frameTime;

        while (accumulator >= PHYSICS_TIMESTEP) {
            world.Step(PHYSICS_TIMESTEP);
            accumulator -= PHYSICS_TIMESTEP;
        }

        // --- Begin Drawing ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // --- Get Body Info ---
        b2Vec2 pos = b2Body_GetPosition(box);
        b2Vec2 pos2 = b2Body_GetPosition(box2);
        b2Vec2 groundPos = b2Body_GetPosition(ground);

        float boxWidth = 1.0f * SCALE;
        float boxHeight = 1.0f * SCALE;
        float groundWidth = 8.0f * SCALE;
        float groundHeight = 2.0f * SCALE;

        // --- Draw Ground ---
        DrawRectangle(
            static_cast<int>(groundPos.x * SCALE - groundWidth / 2),
            static_cast<int>(600 - (groundPos.y * SCALE) - groundHeight / 2),
            static_cast<int>(groundWidth),
            static_cast<int>(groundHeight),
            DARKGRAY
        );

        // --- Draw Box 1 ---
        DrawRectangle(
            static_cast<int>(pos.x * SCALE - boxWidth / 2),
            static_cast<int>(600 - (pos.y * SCALE) - boxHeight / 2),
            static_cast<int>(boxWidth),
            static_cast<int>(boxHeight),
            BLUE
        );

        // --- Draw Box 2 ---
        DrawRectangle(
            static_cast<int>(pos2.x * SCALE - boxWidth / 2),
            static_cast<int>(600 - (pos2.y * SCALE) - boxHeight / 2),
            static_cast<int>(boxWidth),
            static_cast<int>(boxHeight),
            BLUE
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
