#include "raylib.h"
#include "ui.h"
#include "HashTable.h"

int main() {
    // Initialize Raylib window
    InitWindow(800, 600, "Data Structure Visualizer");
    SetTargetFPS(60);
    LoadIcons();

    HashTable hashTable; // Create HashTable instance for testing

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawUI(); // Draw UI

        EndDrawing();
    }

    CloseWindow(); // Close Raylib window
    return 0;
}
