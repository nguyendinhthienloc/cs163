#include "raylib.h"
#include "ui.h"
#include <cstring>
#include <string>

Screen currentScreen = MENU;
float transitionAlpha = 0.0f; // Smooth transition effect
bool transitioning = false;
Screen nextScreen = MENU;

// 🟢 Helper function: Draw smooth buttons
bool DrawButton(Rectangle rect, const char* text, Color color, bool enabled) {
    if (!enabled) color = Fade(color, 0.3f);  // Dim button when disabled

    Vector2 mouse = GetMousePosition();
    bool isHover = enabled && CheckCollisionPointRec(mouse, rect);
    if (isHover) color = Fade(color, 0.6f);

    DrawRectangleRounded(rect, 0.3f, 5, color);
    DrawRectangleRoundedLines(rect, 0.3f, 5, 2, BLACK);
    DrawText(text, rect.x + 20, rect.y + 15, 20, BLACK);

    return enabled && isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}




// 🟢 Smooth Transition Function
void TransitionTo(Screen newScreen) {
    transitioning = true;
    transitionAlpha = 1.0f;
    nextScreen = newScreen;
}

// 🟢 Back Button (Only shown when needed)
void DrawBackButton() {
    if (currentScreen != MENU) {
        if (DrawButton({20, 20, 100, 40}, "Back", GRAY)) TransitionTo(MENU);
    }
}

// 🟢 Main Menu UI
void DrawMenu() {
    ClearBackground(RAYWHITE);
    DrawText("Data Structure Visualizer", 200, 80, 30, DARKBLUE);

    if (DrawButton({250, 200, 300, 50}, "Singly Linked List", SKYBLUE)) TransitionTo(LINKED_LIST);
    if (DrawButton({250, 270, 300, 50}, "Hash Table (Chaining)", ORANGE)) TransitionTo(HASH_TABLE);
    if (DrawButton({250, 340, 300, 50}, "AVL Tree", GREEN)) TransitionTo(AVL_TREE);
    if (DrawButton({250, 410, 300, 50}, "Graph (Shortest Path)", RED)) TransitionTo(GRAPH);
}

// 🟢 Linked List UI
void DrawLinkedList(LinkedList &list) {
    ClearBackground(RAYWHITE);
    DrawBackButton();

    bool hasNodes = list.GetSize() > 0;

    if (DrawButton({600, 100, 180, 50}, "Insert", GREEN, true)) 
        list.Insert(GetRandomValue(1, 99));
    if (DrawButton({600, 160, 180, 50}, "Delete", RED, hasNodes)) 
        list.Delete(GetRandomValue(1, 99));
    if (DrawButton({600, 220, 180, 50}, "Search", BLUE, hasNodes)) 
        list.Search(GetRandomValue(1, 99));

    list.Draw();
}



// 🟢 Hash Table UI
void DrawHashTable(HashTable &hashTable) {
    ClearBackground(RAYWHITE);
    DrawBackButton();

    if (DrawButton({600, 100, 180, 50}, "Insert", GREEN)) hashTable.Insert(GetRandomValue(1, 99));
    if (DrawButton({600, 160, 180, 50}, "Delete", RED)) hashTable.Delete(GetRandomValue(1, 99));
    if (DrawButton({600, 220, 180, 50}, "Search", BLUE)) hashTable.Search(GetRandomValue(1, 99));

    hashTable.Draw();
}

// 🟢 AVL Tree UI
void DrawAVLTree(AVLTree &avlTree) {
    ClearBackground(RAYWHITE);
    DrawBackButton();

    if (DrawButton({600, 100, 180, 50}, "Insert", GREEN)) avlTree.Insert(GetRandomValue(1, 99));
    if (DrawButton({600, 160, 180, 50}, "Delete", RED)) avlTree.Delete(GetRandomValue(1, 99));
    if (DrawButton({600, 220, 180, 50}, "Search", BLUE)) avlTree.Search(GetRandomValue(1, 99));

    avlTree.Draw();
}

// 🟢 Graph UI
void DrawGraph(Graph &graph) {
    ClearBackground(RAYWHITE);
    DrawBackButton();
    graph.Draw();
}

// 🟢 Main UI Dispatcher
void DrawUI(LinkedList &list, HashTable &hashTable, AVLTree &avlTree, Graph &graph) {
    if (transitioning) {
        transitionAlpha -= 0.05f;
        if (transitionAlpha <= 0.0f) {
            currentScreen = nextScreen;
            transitioning = false;
        }
    }

    if (currentScreen == MENU) DrawMenu();
    else {
        DrawBackButton();
        if (currentScreen == LINKED_LIST) DrawLinkedList(list);
        else if (currentScreen == HASH_TABLE) DrawHashTable(hashTable);
        else if (currentScreen == AVL_TREE) DrawAVLTree(avlTree);
        else if (currentScreen == GRAPH) DrawGraph(graph);
    }

    if (transitioning) DrawRectangle(0, 0, 800, 600, Fade(BLACK, transitionAlpha));
}
