#include "linked_list.h"
#include <iostream>
#include <algorithm>

LinkedList::~LinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    nodes.clear();
}

int LinkedList::GetSize() { return nodes.size(); }

void LinkedList::Insert(int value) {
    float startX = 120 + static_cast<float>(nodes.size()) * nodeSpacing;
    Node* newNode = new Node(value, { startX, 400 });

    animState = AnimState::INSERTING;
    animProgress = 0.0f;
    animNode = newNode;
    animStartPos = { startX, 200 };
    newNode->position = animStartPos;

    if (!head) {
        head = newNode;
        cur = head;
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
        cur = temp;
    }
    nodes.push_back(newNode);
}

void LinkedList::Delete(int value) {
    if (!head) {
        std::cout << "List is empty, nothing to delete!" << std::endl;
        return;
    }

    Node* temp = head;
    Node* prev = nullptr;
    cur = head;

    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) {
        std::cout << "Value " << value << " not found for deletion!" << std::endl;
        return;
    }

    animState = AnimState::DELETING;
    animProgress = 0.0f;
    animNode = temp;
    animStartPos = temp->position;

    if (prev) {
        prev->next = temp->next;
    } else {
        head = temp->next;
    }

    auto it = std::find(nodes.begin(), nodes.end(), temp);
    if (it != nodes.end()) {
        nodes.erase(it);
    }
    cur = nullptr;
}

bool LinkedList::Search(int value) {
    if (!head) return false;

    foundNode = nullptr;
    cur = head;
    curPos = head->position;  // Start at head’s position
    animState = AnimState::SEARCHING;
    animProgress = 0.0f;
    animNode = new Node(value, {0, 0});
    searchAttempted = true;

    return true;
}

void LinkedList::Update(int oldValue, int newValue) {
    for (auto node : nodes) {
        if (node->value == oldValue) {
            node->value = newValue;
            break;
        }
    }
}

Node* LinkedList::GetNodeAt(int index) {
    if (index < 0 || index >= GetSize()) return nullptr;
    Node* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return current;
}