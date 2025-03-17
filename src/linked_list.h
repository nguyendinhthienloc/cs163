// linked_list.h - Singly Linked List Visualization
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include <vector>

// Node structure
struct Node {
    int value;
    Vector2 position;
    Node* next;
    
    Node(int val, Vector2 pos) : value(val), position(pos), next(nullptr) {}
};

// Linked List Class
class LinkedList {
private:

    Node* head;
    Node* foundNode; // Stores last found node
    std::vector<Node*> nodes;
    float nodeSpacing = 80; // Distance between nodes

public:
    LinkedList();
    ~LinkedList();
    void Insert(int value);
    void Delete(int value);
    void Search(int value);
    void Update(int oldValue, int newValue);
    void Draw();
};

#endif // LINKED_LIST_H