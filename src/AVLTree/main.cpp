#include "AVLtreeAnimation.h"
#include "raylib.h"

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "AVL Tree Visualizer");
    SetTargetFPS(60);

    AVLTreeVisualizer visualizer;

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();
        visualizer.handleInput();
        visualizer.updateAnimation(deltaTime);
        BeginDrawing();
        visualizer.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}