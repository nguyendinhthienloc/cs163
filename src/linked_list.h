#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include <vector>
#include <cmath>

// Node structure
struct Node {
    int value;
    Vector2 position;
    Vector2 targetPosition;  // Smooth transition target
    Node* next;
    
    Node(int val, Vector2 pos) : value(val), position(pos), targetPosition(pos), next(nullptr) {}
};

// Linked List Class
class LinkedList {
private:
    Node* head;
    Node* foundNode; // Stores last found node
    std::vector<Node*> nodes;
    float nodeSpacing = 100; // Dynamic spacing

    void RecalculatePositions();
    Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius);  // 🔥 ADDED FUNCTION for precise arrow positioning

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
