#include "raylib.h"
#include "../include/hashtable_ui.h"

int main() {
    // Initialize the Raylib window
    InitWindow(1600, 900, "Data Structure Visualizer");
    SetTargetFPS(60);

    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawHashTable();
        
        EndDrawing();
    }

    // Close window when exiting
    CloseWindow();
    return 0;
}
