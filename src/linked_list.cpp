// linked_list.cpp - Singly Linked List Visualization
#include "linked_list.h"
#include "ui.h"  // Ensure this is included for DrawButton
#include <iostream>
#include <algorithm>
#include "raylib.h"

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

// Insert a new node
void LinkedList::Insert(int value) {
    Node* newNode = new Node(value, {0, 0});  // Temporarily set position
    if (!head) {
        head = newNode;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
    nodes.push_back(newNode);

    // Reposition all nodes
    float xPos = 100;
    for (auto node : nodes) {
        node->position = {xPos, 300};
        xPos += nodeSpacing;
    }
}

// Delete a node with a specific value
void LinkedList::Delete(int value) {
    if (!head) return;

    Node* temp = head;
    Node* prev = nullptr;

    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) return; // Value not found

    if (prev) {
        prev->next = temp->next;
    } else {
        head = temp->next;
    }

    // Remove from vector BEFORE deleting the node
    nodes.erase(std::remove(nodes.begin(), nodes.end(), temp), nodes.end());

    // Delete the node safely
    delete temp;

    // Reposition nodes after deletion
    float xPos = 100;
    for (auto node : nodes) {
        node->position = {xPos, 300};
        xPos += nodeSpacing;
    }
}


// Search for a node and highlight it
void LinkedList::Search(int value) {
    foundNode = nullptr;
    for (auto node : nodes) {
        if (node->value == value) {
            foundNode = node;
            break;
        }
    }
}

// Update a node’s value
void LinkedList::Update(int oldValue, int newValue) {
    for (auto node : nodes) {
        if (node->value == oldValue) {
            node->value = newValue;
            break;
        }
    }
}

// Draw the linked list
void LinkedList::Draw() {
    for (auto node : nodes) {
        // Smooth movement using interpolation
        Vector2 targetPos = {100 + (nodeSpacing * std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node))), 300};
        node->position.x += (targetPos.x - node->position.x) * 0.1f; // LERP for smooth movement
        node->position.y += (targetPos.y - node->position.y) * 0.1f;

        DrawCircleV(node->position, 30, (node == foundNode) ? GREEN : SKYBLUE);
        DrawText(TextFormat("%d", node->value), node->position.x - 10, node->position.y - 10, 20, BLACK);

        if (node->next) {
            DrawLineV(node->position, node->next->position, BLACK);
        }
    }
}


// UI for linked list operations
void DrawLinkedListUI(LinkedList &list) {
    if (DrawButton({600, 100, 180, 50}, "Insert", GREEN)) {
        list.Insert(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 160, 180, 50}, "Delete", RED)) {
        list.Delete(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 220, 180, 50}, "Search", BLUE)) {
        list.Search(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 280, 180, 50}, "Update", ORANGE)) {
        list.Update(GetRandomValue(1, 99), GetRandomValue(1, 99));
    }
    list.Draw();
}
