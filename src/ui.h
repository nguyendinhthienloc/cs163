#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "linked_list.h"
#include "hash_table.h"
#include "avl_tree.h"
#include "graph.h"

// Enum defining different screens
typedef enum {
    MENU,
    LINKED_LIST,
    HASH_TABLE,
    AVL_TREE,
    GRAPH
} Screen;

// Global screen tracker
extern Screen currentScreen;

// UI Functions
void DrawUI(LinkedList &list, HashTable &hashTable, AVLTree &avlTree, Graph &graph);
void DrawMenu();
void DrawLinkedList(LinkedList &list);
void DrawHashTable(HashTable &hashTable);
void DrawAVLTree(AVLTree &avlTree);
void DrawGraph(Graph &graph);
void DrawBackButton();
bool DrawButton(Rectangle rect, const char* text, Color color);

// 🟢 Smooth screen transition function
void TransitionTo(Screen newScreen);

#endif // UI_H
