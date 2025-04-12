#include "raylib.h"
#include <string>
#include <vector>
#include <cmath>


#include "../header/ui_menu.h"
#include "../header/Color.h"

#include "../header/Linked List/linked_list.h"
#include "../header/Linked List/ui_linked_list.h"

#include "../header/AVL Tree/AVLtree.h"
#include "../header/AVL Tree/AVLtreeAnimation.h"
#include "../header/AVL Tree/initAVLProgram.h"

#include "../header/Hash Table/HashTable.h"
#include "../header/Hash Table/hashtable_ui.h"

#include "../header/Graph/Graph.h"
#include "../header/Graph/ui_Graph.h"


bool showGreeting = true;
float fadeAlpha = 1.0f;
bool isFullScreen = false;

void DrawUI(LinkedList& list, AVLTreeVisualizer& AVLvisualizer, bool& isFullScreen, InputState& inputState);

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Data Structure Visualizer");
    SetTargetFPS(60);

    LinkedList linkedList;
    AVLTreeVisualizer AVLvisualizer;
    InputState inputState = { {0}, 0, false };
    codeFont = LoadFont("Courier-Bold.ttf");
	float time = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(CreamyBlueBG);

        if (showGreeting) {
            int width1 = MeasureText("Welcome to Group 11's Data Visualizer!", 60);
            int width2 = MeasureText("Press ENTER to continue...", 40);

			time += GetFrameTime();
            float sparkleAlpha = (sin(time * 2.5f) + 1.0f) / 2.0f;
			Color sparkleColor = Fade(DARKGRAY, sparkleAlpha);

            DrawText("Welcome to Group 11's Data Visualizer!", (SCREEN_WIDTH - width1) / 2, 250, 60, DARKBLUE);
            DrawText("Press ENTER to continue...", (SCREEN_WIDTH - width2) / 2, 400, 40, sparkleColor);

            if (IsKeyPressed(KEY_ENTER)) {
                showGreeting = false;
            }
        } 
        
        else {
            if (fadeAlpha > 0.0f) fadeAlpha -= 0.02f;
           
            DrawUI(linkedList, AVLvisualizer, isFullScreen, inputState);

            if (fadeAlpha > 0.0f) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, fadeAlpha));
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawUI(LinkedList& list, AVLTreeVisualizer& AVLvisualizer, bool& isFullScreen, InputState& inputState) {
    static bool isDragging = false;
    std::string feedbackMessage = "";
    double feedbackTimer = 0.0;
    float dragStartX = 0.0f;

    list.UpdateAnimation(GetFrameTime());

    if (transitioning) {
        transitionAlpha -= 0.5f;
        if (transitionAlpha <= 0.0f) {
            currentScreen = nextScreen;
            transitioning = false;
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
            inputState.inputActive = false;
        }
    }

    if (IsKeyPressed(KEY_F11)) {
        /*isFullScreen = !isFullScreen;
        if (isFullScreen) {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        }
        else {
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            ToggleFullscreen();
        }*/
		ToggleFullscreen();
    }

    if (currentScreen == MENU) {
        ClearBackground(CreamyBlueBG);
        DrawMenu();
    }
    else if (currentScreen == LINKED_LIST) {
        ClearBackground(CreamyBlueBG);
        DrawLinkedList(list, inputState, feedbackMessage, feedbackTimer, isDragging, dragStartX); 
        DrawBackButton();
    }

    else if (currentScreen == HASH_TABLE) {
        DrawHashTable();
        DrawBackButton();
    }

    else if (currentScreen == AVL_TREE) {
        ClearBackground(CreamyBlueBG);
        float deltaTime = GetFrameTime();
        AVLvisualizer.handleInput();
        AVLvisualizer.updateAnimation(deltaTime);
        AVLvisualizer.draw();
        DrawBackButton();
    }

    else if (currentScreen == GRAPH) {
        ClearBackground(CreamyBlueBG);
        DrawGraphProgram();
        DrawBackButton();
    }
}