#include "linked_list.h"
#include <iostream>
#include <algorithm>

// Constructor
LinkedList::LinkedList() : head(nullptr), foundNode(nullptr) {}

// Destructor
LinkedList::~LinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    nodes.clear();
}

// Insert a new node with smooth animation
void LinkedList::Insert(int value) {
    float startX = 120 + nodes.size() * nodeSpacing;
    Node* newNode = new Node(value, {startX, 300});  // Start at computed position

    if (!head) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }

    nodes.push_back(newNode);
    RecalculatePositions();
}


// Delete a node smoothly
void LinkedList::Delete(int value) {
    if (!head) return;

    Node* temp = head;
    Node* prev = nullptr;

    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return;  // Node not found, exit early

    if (prev) prev->next = temp->next;
    else head = temp->next;

    auto it = std::find(nodes.begin(), nodes.end(), temp);
    if (it != nodes.end()) {
        nodes.erase(it);
        delete temp;  // Properly delete node
    }

    RecalculatePositions();
}




// Search for a node and highlight it smoothly
void LinkedList::Search(int value) {
    foundNode = nullptr;
    for (auto node : nodes) {
        if (node->value == value) {
            foundNode = node;
            break;
        }
    }
}

// Update a node’s value smoothly
void LinkedList::Update(int oldValue, int newValue) {
    for (auto node : nodes) {
        if (node->value == oldValue) {
            node->value = newValue;
            break;
        }
    }
}

// Recalculate positions with dynamic spacing
void LinkedList::RecalculatePositions() {
    float screenWidth = 800;
    float maxNodes = 8;
    float startX = 120;
    float yPos = 300;

    nodeSpacing = (nodes.size() > maxNodes) ? screenWidth / nodes.size() : 100;
    if (nodeSpacing < 50) nodeSpacing = 50; // Prevents nodes from overlapping too much

    for (auto node : nodes) {
        node->targetPosition = {startX, yPos};
        startX += nodeSpacing;
    }
}



// Helper function to calculate direction vector
Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius) {
    Vector2 direction = {to.x - from.x, to.y - from.y};
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    
    if (length == 0) return from;  // Avoid division by zero

    direction.x /= length;
    direction.y /= length;

    return {from.x + direction.x * radius, from.y + direction.y * radius};
}


// Draw the linked list with correctly positioned arrows
void LinkedList::Draw() {
    float radius = 30;
    float lerpSpeed = 0.1f;  // Adjustable for smooth movement
    bool flash = (GetTime() - (int)GetTime() < 0.5f);  // Toggle color every 0.5 seconds

    for (auto node : nodes) {
        // Dynamic LERP adjustment based on distance
        float dist = sqrt(pow(node->targetPosition.x - node->position.x, 2) +
                          pow(node->targetPosition.y - node->position.y, 2));
        float dynamicLerp = (dist > 50) ? 0.15f : lerpSpeed;  // Increase speed if far apart

        node->position.x += (node->targetPosition.x - node->position.x) * dynamicLerp;
        node->position.y += (node->targetPosition.y - node->position.y) * dynamicLerp;

        // 🎯 Apply flashing effect for search result node
        Color nodeColor = (node == foundNode && flash) ? RED : SKYBLUE;
        DrawCircleV(node->position, radius, nodeColor);
        DrawText(TextFormat("%d", node->value), node->position.x - 10, node->position.y - 10, 20, BLACK);

        // Draw Arrows
        if (node->next) {
            Vector2 startArrow = CalculateArrowStart(node->position, node->next->position, radius);
            Vector2 endArrow = CalculateArrowStart(node->next->position, node->position, -radius);  

            DrawLineEx(startArrow, endArrow, 3.0f, BLACK);
        }
    }
}


