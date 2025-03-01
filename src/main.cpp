#include "raylib.h"
#include "ui.h"

int main() {
    // Initialize the Raylib window
    InitWindow(800, 600, "Data Structure Visualizer");
    SetTargetFPS(60);

    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawUI();  // Calls the correct screen rendering function
        
        EndDrawing();
    }

    // Close window when exiting
    CloseWindow();
    return 0;
}
