#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>


struct Node {
    int value;
    Vector2 position;
    Vector2 targetPosition;
    Node* next;
    float alpha;

    Node(int val, Vector2 pos) : value(val), position(pos), targetPosition(pos), next(nullptr), alpha(1.0f) {}
};

class LinkedList {
private:
    Vector2 curPos;  // Current animation position
    Node* head;
    Node* foundNode;
    Node* cur;
    std::vector<Node*> nodes;
    float nodeSpacing = 100;
    float scrollOffsetX = 0.0f;
    float scrollOffsetY = 0.0f;
    bool searchAttempted = false;

    // Animation state (moved to animation file but declared here)
    enum class AnimState { IDLE, INSERTING, DELETING, SEARCHING };
    AnimState animState = AnimState::IDLE;
    float animProgress = 0.0f;
    Node* animNode = nullptr;
    Vector2 animStartPos;
    float traversalSpeed = 0.1f;
    float highlightTimer = 0.0f;

    // Private methods (split across files)
    void RecalculatePositions();
    Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius);
    Vector2 CalculateArrowEnd(Vector2 from, Vector2 to, float radius);

public:
    LinkedList() : head(nullptr), foundNode(nullptr), cur(nullptr), scrollOffsetX(0), scrollOffsetY(0), animStartPos{0, 0}, curPos{0, 0} {}
    ~LinkedList();

    // Core operations
    int GetSize();
    void Insert(int value);
    void Delete(int value);
    bool Search(int value);
    void Update(int oldValue, int newValue);
    Node* GetNodeAt(int index);

    // Animation and drawing
    void UpdateAnimation(float deltaTime);
    void Draw();

    // Scroll getters/setters
    float GetScrollOffsetX() const { return scrollOffsetX; }
    void SetScrollOffsetX(float offset) { scrollOffsetX = offset; }
    float GetScrollOffsetY() const { return scrollOffsetY; }
    void SetScrollOffsetY(float offset) { scrollOffsetY = offset; }

    // Animation state
    bool IsAnimating() const { return animState != AnimState::IDLE; }
    Node* GetFoundNode() const { return foundNode; }
    void SetFoundNode(Node* node) { foundNode = node; }
};

#endif // LINKED_LIST_H