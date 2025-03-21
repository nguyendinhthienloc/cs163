#include "raylib.h"
#include "ui_menu.h"
#include "ui_linked_list.h"
#include "linked_list.h"
#include <string>

bool showGreeting = true;
float fadeAlpha = 1.0f;
bool isFullScreen = false;


void DrawUI(LinkedList& list, bool& isFullScreen, InputState& inputState) {
    static bool isDragging = false;
    std::string feedbackMessage = "";
    double feedbackTimer = 0.0;
    float dragStartX = 0.0f;

    list.UpdateAnimation(GetFrameTime());

    if (transitioning) {
        transitionAlpha -= 0.05f;
        if (transitionAlpha <= 0.0f) {
            currentScreen = nextScreen;
            transitioning = false;
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
            inputState.inputActive = false;
        }
    }

    if (IsKeyPressed(KEY_F11)) {
        isFullScreen = !isFullScreen;
        if (isFullScreen) {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        } else {
            SetWindowSize(800, 600);
            ToggleFullscreen();
        }
    }

    if (currentScreen == MENU) {
        DrawMenu();
    } else if (currentScreen == LINKED_LIST) {
        DrawLinkedList(list, inputState, feedbackMessage, feedbackTimer, isDragging, dragStartX);
    }

    if (transitioning) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transitionAlpha));
    }
}

int main() {
    InitWindow(800, 600, "Linked List Visualizer");
    SetTargetFPS(60);

    LinkedList linkedList;
    InputState inputState = { {0}, 0, false };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (showGreeting) {
            DrawText("Welcome to Linked List Visualizer!", 120, 250, 30, DARKBLUE);
            DrawText("Press ENTER to continue...", 250, 350, 20, DARKGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                showGreeting = false;
            }
        } else {
            if (fadeAlpha > 0.0f) fadeAlpha -= 0.02f;

            DrawUI(linkedList, isFullScreen, inputState);

            if (fadeAlpha > 0.0f) {
                DrawRectangle(0, 0, 800, 600, Fade(BLACK, fadeAlpha));
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}