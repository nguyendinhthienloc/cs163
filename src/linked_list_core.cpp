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
    Node* newNode = new Node(value, { startX, 400 });  // Updated yPos to 400

    animState = AnimState::INSERTING;
    animProgress = 0.0f;
    animNode = newNode;
    animStartPos = { startX, 400 };
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

    // Add to history
    history.push_back(Operation(Operation::Type::INSERT, value));
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

    // Add to history with position and prev node
    history.push_back(Operation(Operation::Type::DELETE, value, temp->position, prev));
}

bool LinkedList::Search(int value) {
    if (!head) return false;

    foundNode = nullptr;
    cur = head;
    curPos = head->position;
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

void LinkedList::Undo() {
    if (history.empty() || IsAnimating()) return;  // No history or animation in progress

    Operation lastOp = history.back();
    history.pop_back();  // Remove the operation immediately

    if (lastOp.type == Operation::Type::INSERT) {
        // Undo insert: delete the last inserted value
        Node* temp = head;
        Node* prev = nullptr;
        while (temp && temp->value != lastOp.value) {
            prev = temp;
            temp = temp->next;
        }
        if (temp) {
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
    } else if (lastOp.type == Operation::Type::DELETE) {
        // Undo delete: re-insert the node at its original position
        Node* newNode = new Node(lastOp.value, lastOp.position);
        animState = AnimState::INSERTING;
        animProgress = 0.0f;
        animNode = newNode;
        animStartPos = { lastOp.position.x, 200 };
        newNode->position = animStartPos;

        if (!head || !lastOp.prevNode) {
            newNode->next = head;
            head = newNode;
        } else {
            newNode->next = lastOp.prevNode->next;
            lastOp.prevNode->next = newNode;
        }
        nodes.push_back(newNode);
    }
}