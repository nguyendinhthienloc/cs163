#include "raylib.h"
#include "ui.h"
#include "linked_list.h"
#include "hash_table.h"
#include "avl_tree.h"
#include "graph.h"

// 🟢 State tracking for smooth screen transitions
bool showGreeting = true;
float fadeAlpha = 1.0f; // Used for fade-in effect

int main() {
    // 🟢 Initialize Raylib Window
    InitWindow(800, 600, "Data Structure Visualizer");
    SetTargetFPS(60);

    // 🟢 Initialize Data Structures
    LinkedList linkedList;
    HashTable hashTable;
    AVLTree avlTree;
    Graph graph;

    // 🟢 Main Loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 🟢 Greeting Screen (first-time launch)
        if (showGreeting) {
            DrawText("Welcome to Data Structure Visualizer!", 120, 250, 30, DARKBLUE);
            DrawText("Press ENTER to continue...", 250, 350, 20, DARKGRAY);
            
            if (IsKeyPressed(KEY_ENTER)) {
                showGreeting = false; // Move to menu
            }
        } else {
            // 🟢 Apply Fade Effect for Smooth Transitions
            if (fadeAlpha > 0.0f) fadeAlpha -= 0.02f; // Decrease fade effect smoothly

            DrawUI(linkedList, hashTable, avlTree, graph);

            // Fade Effect Overlay (Smooth UI Transition)
            if (fadeAlpha > 0.0f) {
                DrawRectangle(0, 0, 800, 600, Fade(BLACK, fadeAlpha));
            }
        }

        EndDrawing();
    }

    // 🟢 Cleanup Before Exiting
    CloseWindow();
    return 0;
}
