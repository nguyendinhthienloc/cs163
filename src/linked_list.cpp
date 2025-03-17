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
    Node* newNode = new Node(value, {0, 300});  // Start off-screen
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

    if (!temp) return;  // Value not found

    if (prev) {
        prev->next = temp->next;
    } else {
        head = temp->next;
    }

    // Remove node from visualization list
    nodes.erase(std::remove(nodes.begin(), nodes.end(), temp), nodes.end());

    delete temp;  // Free memory
    RecalculatePositions();  // Adjust UI layout
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
    float screenWidth = 800;  // Set your screen width
    float maxNodes = 8;  // Maximum nodes before spacing changes
    float startX = 120;
    float yPos = 300;
    nodeSpacing = (nodes.size() > maxNodes) ? screenWidth / nodes.size() : 100;

    for (auto node : nodes) {
        node->targetPosition = {startX, yPos};
        startX += nodeSpacing;
    }
}

// Helper function to calculate direction vector
Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius) {
    float angle = atan2f(to.y - from.y, to.x - from.x);
    return {from.x + cosf(angle) * radius, from.y + sinf(angle) * radius};
}

// Draw the linked list with correctly positioned arrows
void LinkedList::Draw() {
    float radius = 30;  // Circle radius

    for (auto node : nodes) {
        // LERP for smooth movement
        node->position.x += (node->targetPosition.x - node->position.x) * 0.1f;
        node->position.y += (node->targetPosition.y - node->position.y) * 0.1f;

        // Draw Node
        DrawCircleV(node->position, radius, (node == foundNode) ? GREEN : SKYBLUE);
        DrawText(TextFormat("%d", node->value), node->position.x - 10, node->position.y - 10, 20, BLACK);

        // Draw Correctly Positioned Arrows
        if (node->next) {
            Vector2 startArrow = CalculateArrowStart(node->position, node->next->position, radius);
            Vector2 endArrow = CalculateArrowStart(node->next->position, node->position, -radius);  // Arrow should end just outside the next node

            DrawLineEx(startArrow, endArrow, 3.0f, BLACK);  // Smooth line with correct thickness
        }
    }
}
