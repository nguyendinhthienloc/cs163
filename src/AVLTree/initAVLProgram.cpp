#include "AVLtreeAnimation.h"
#include "raylib.h"
#include "Color.h"
#include "initAVLProgram.h"

void initAVLProgram() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "AVL Tree Visualizer");
    SetTargetFPS(60);

    AVLTreeVisualizer visualizer;

    while (!WindowShouldClose()) {
        ClearBackground(CreamyBlueBG);
        float deltaTime = GetFrameTime();
        visualizer.handleInput();
        visualizer.updateAnimation(deltaTime);
        BeginDrawing();
        visualizer.draw();
        EndDrawing();
    }

    CloseWindow();
}

