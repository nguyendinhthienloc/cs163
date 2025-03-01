#include "LinkedList.h"
#include <iostream>
#include <algorithm>
#include <cmath> // Required for fmin

float yStart = std::fmin((float)GetMonitorHeight(0) / 2.5, (float)GetMonitorHeight(0) - 100);


// Linear interpolation function for smooth animation
float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Constructor
LinkedList::LinkedList() {
    head = nullptr;
}

// Destructor to free memory
LinkedList::~LinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// Update positions of nodes, keeping them within the screen bounds
void LinkedList::updatePositions() {
    float xStart = 100;
    float yStart = std::fmin((float)GetMonitorHeight(0) / 2.5, (float)GetMonitorHeight(0) - 100); // Prevent going off-screen
    float spacing = 120;
    float maxX = GetMonitorWidth(0) - 50; // Keep within screen width

    for (size_t i = 0; i < nodes.size(); i++) {
        nodes[i]->position = { std::min(xStart + (i * spacing), maxX), yStart };
    }
}

// Insert a node at the end
void LinkedList::insert(int value) {
    Node* newNode = new Node(value, {0, 0});
    if (!head) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
    nodes.push_back(newNode);
    updatePositions();
}

// Remove a node by value
void LinkedList::remove(int value) {
    if (!head) return;
    
    Node* temp = head;
    Node* prev = nullptr;

    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return;

    if (prev) {
        prev->next = temp->next;
    } else {
        head = head->next;
    }

    nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [temp](Node* n) { return n == temp; }), nodes.end());

    delete temp;
    updatePositions();
}

// Get the number of nodes
int LinkedList::getSize() {
    return nodes.size();
}

// Draw nodes with smooth animations
void LinkedList::draw() {
    for (size_t i = 0; i < nodes.size(); i++) {
        Node* node = nodes[i];
        node->position.x = Lerp(node->position.x, 100 + (i * 120), 0.2f); // Smoother animation

        DrawCircleV(node->position, 28, DARKBLUE);
        DrawCircleV(node->position, 25, BLUE);
        DrawText(TextFormat("%d", node->value), node->position.x - 10, node->position.y - 10, 20, WHITE);

        if (i < nodes.size() - 1) {
            DrawLineEx(node->position, nodes[i + 1]->position, 3, WHITE);
        }
    }
}
