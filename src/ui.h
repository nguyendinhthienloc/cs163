#ifndef UI_H
#define UI_H

class HashTable;

extern HashTable ht;

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

// Icons for undo and redo buttons
void LoadIcons();

// Function prototypes
void DrawMenu();
//void DrawLinkedList();
void DrawHashTable();
//void DrawTwoThreeTree();
//void DrawGraph();
void DrawUI();

#endif // UI_H
