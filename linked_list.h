#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include <vector>
#include <cmath>
#include <string>

struct Node {
    int value;
    Vector2 position;
    Vector2 targetPosition;
    Node* next;
    float alpha;  // For fade animations

    Node(int val, Vector2 pos) : value(val), position(pos), targetPosition(pos), next(nullptr), alpha(1.0f) {}
};

class LinkedList {
private:
    Node* head;
    Node* foundNode;  // To store the node found by the search
    Node* cur;  // Current pointer for animations
    std::vector<Node*> nodes;
    float nodeSpacing = 100;
    float scrollOffsetX = 0.0f;
    float scrollOffsetY = 0.0f; // New vertical offset
    bool searchAttempted = false;  // New flag to track search attempts

    // Animation states
    enum class AnimState { IDLE, INSERTING, DELETING, SEARCHING };
    AnimState animState = AnimState::IDLE;
    float animProgress = 0.0f;
    Node* animNode = nullptr;
    Vector2 animStartPos;

    void RecalculatePositions();
    Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius);
    Vector2 CalculateArrowEnd(Vector2 from, Vector2 to, float radius);

public:
    LinkedList();
    ~LinkedList();

    int GetSize() { return nodes.size(); }
    void Insert(int value);
    void Delete(int value);
    bool Search(int value);  // Return whether the node was found
    void Update(int oldValue, int newValue);
    void Draw();
    void UpdateAnimation(float deltaTime);  // New: Animation update
    float GetScrollOffsetX() const { return scrollOffsetX; }
    void SetScrollOffsetX(float offset) { scrollOffsetX = offset; }
    float GetScrollOffsetY() const { return scrollOffsetY; }
    void SetScrollOffsetY(float offset) { scrollOffsetY = offset; }
    bool IsAnimating() const { return animState != AnimState::IDLE; }

    Node* GetNodeAt(int index);  // Only the declaration, not the definition
    Node* GetFoundNode() const { return foundNode; }  // New: Get the found node for highlighting
    void SetFoundNode(Node* node) { foundNode = node; }  // New: Set the found node
};

#endif
