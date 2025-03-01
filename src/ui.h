#ifndef UI_H
#define UI_H

#include "raylib.h"

// Enumeration for different screens
typedef enum {
    MENU,
    LINKED_LIST,
    HASH_TABLE,
    TWO_THREE_TREE,
    GRAPH
} Screen;

// Global screen variable
extern Screen currentScreen;

// Function prototypes
void DrawMenu();
void DrawLinkedList();
void DrawHashTable();
void DrawTwoThreeTree();
void DrawGraph();
void DrawUI();

#endif // UI_H
