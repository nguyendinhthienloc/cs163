// ui.cpp - UI and Screen Management
#include "raylib.h"
#include "ui.h"
#include "linked_list.h"
#include <cstring>// Draw Linked List UI (fixing undefined function issue)
#include <string>

LinkedList linkedList; // Global LinkedList instance

Screen currentScreen = MENU;

// Button rendering function
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

// Draw Back Button for navigation
void DrawBackButton() {
    if (DrawButton({20, 20, 100, 40}, "Back", GRAY)) {
        currentScreen = MENU;
    }
}



// User input text box
char inputText[10] = "";
int inputValue = 0;
bool inputActive = false;

void DrawLinkedListUI(LinkedList &list) {
    DrawText("Enter value:", 600, 50, 20, BLACK);
    Rectangle inputBox = {600, 70, 180, 40};
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawText(inputText, inputBox.x + 10, inputBox.y + 10, 20, BLACK);

    // Handle user input
    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = true;
    }
    if (inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= '0' && key <= '9') && (strlen(inputText) < 9)) {
                int len = strlen(inputText);
                inputText[len] = (char)key;
                inputText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(inputText) > 0) {
            inputText[strlen(inputText) - 1] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER)) {
            inputValue = atoi(inputText);
            inputActive = false;
        }
    }

    if (DrawButton({600, 130, 180, 50}, "Insert", GREEN)) {
        if (inputValue != 0) {
            list.Insert(inputValue);
        }
    }
    if (DrawButton({600, 190, 180, 50}, "Delete", RED)) {
        if (inputValue != 0) {
            list.Delete(inputValue);
        }
    }
    if (DrawButton({600, 250, 180, 50}, "Search", BLUE)) {
        if (inputValue != 0) {
            list.Search(inputValue);
        }
    }
    if (DrawButton({600, 310, 180, 50}, "Update", ORANGE)) {
        if (inputValue != 0) {
            int newValue = GetRandomValue(1, 99); // Allowing a new random value
            list.Update(inputValue, newValue);
        }
    }
    list.Draw();
}


// Draw Linked List Screen
void DrawLinkedList(LinkedList &list) {
    ClearBackground(RAYWHITE);
    DrawBackButton();
    DrawLinkedListUI(list);
}

// Draw UI based on the current screen
void DrawUI(LinkedList &list) {
    if (currentScreen == MENU) {
        DrawMenu();
    } else {
        DrawBackButton();
        if (currentScreen == LINKED_LIST) {
            DrawLinkedList(list);
        }
    }
}
