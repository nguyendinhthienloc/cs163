#include "raylib.h"
#include "ui.h"

// Define screen variable (default: MENU)
Screen currentScreen = MENU;

// Button rendering function with animations
bool DrawButton(Rectangle rect, const char* text, Color color) {
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);
    
    DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
    DrawRectangleLinesEx(rect, 2, BLACK);
    DrawText(text, rect.x + 20, rect.y + 15, 20, BLACK);

    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

// Draw Main Menu UI
void DrawMenu() {
    ClearBackground(RAYWHITE);
    DrawText("Data Structure Visualizer", 180, 80, 30, DARKBLUE);
    
    if (DrawButton({250, 200, 300, 50}, "Singly Linked List", SKYBLUE)) {
        currentScreen = LINKED_LIST;
    }
    if (DrawButton({250, 270, 300, 50}, "Hash Table (Chaining)", ORANGE)) {
        currentScreen = HASH_TABLE;
    }
    if (DrawButton({250, 340, 300, 50}, "2-3 Tree", GREEN)) {
        currentScreen = TWO_THREE_TREE;
    }
    if (DrawButton({250, 410, 300, 50}, "Shortest Path (Graph)", RED)) {
        currentScreen = GRAPH;
    }
}

// Draw Back Button on visualization screens
void DrawBackButton() {
    if (DrawButton({20, 20, 100, 40}, "Back", GRAY)) {
        currentScreen = MENU;
    }
}

// Draw UI based on the current screen
void DrawUI() {
    if (currentScreen == MENU) {
        DrawMenu();
    } else {
        DrawBackButton(); // Adds a back button for navigation
        if (currentScreen == LINKED_LIST) {
            DrawLinkedList();
        } else if (currentScreen == HASH_TABLE) {
            DrawHashTable();
        } else if (currentScreen == TWO_THREE_TREE) {
            DrawTwoThreeTree();
        } else if (currentScreen == GRAPH) {
            DrawGraph();
        }
    }
}