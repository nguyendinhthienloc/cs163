#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "raylib.h"
#include <vector>

struct Node {
    int value;
    Node* next;
    Vector2 position;
    Node(int val, Vector2 pos) : value(val), next(nullptr), position(pos) {}
};

class LinkedList {
private:
    Node* head;
    std::vector<Node*> nodes;
    void updatePositions();

public:
    LinkedList();
    ~LinkedList();
    
    void insert(int value);
    void remove(int value);
    void draw();
    int getSize();  // ✅ Added to return node count
};

#endif
