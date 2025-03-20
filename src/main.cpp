#include "raylib.h"
#include "ui.h"
#include "linked_list.h"
#include <string>

bool showGreeting = true;
float fadeAlpha = 1.0f;
bool isFullScreen = false;

int main() {
    InitWindow(800, 600, "Linked List Visualizer");
    SetTargetFPS(60);

    LinkedList linkedList;
    InputState inputState;  // Declare the InputState variable

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (showGreeting) {
            DrawText("Welcome to Linked List Visualizer!", 120, 250, 30, DARKBLUE);
            DrawText("Press ENTER to continue...", 250, 350, 20, DARKGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                showGreeting = false;
            }
        }
        else {
            if (fadeAlpha > 0.0f) fadeAlpha -= 0.02f;

            DrawUI(linkedList, isFullScreen, inputState);  // Pass inputState to DrawUI

            if (fadeAlpha > 0.0f) {
                DrawRectangle(0, 0, 800, 600, Fade(BLACK, fadeAlpha));
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
