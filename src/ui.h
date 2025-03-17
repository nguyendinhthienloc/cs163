#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "linked_list.h"


// Enum for different screens
typedef enum {
    MENU,
    LINKED_LIST,
    HASH_TABLE,
    TWO_THREE_TREE,
    GRAPH
} Screen;

// Global variable for screen state
extern Screen currentScreen;

// UI Functions
void DrawMenu();
void DrawLinkedList(LinkedList &list);


// Button Helper Function
bool DrawButton(Rectangle rect, const char* text, Color color);

#endif // UI_H
// End of Path: ui.h