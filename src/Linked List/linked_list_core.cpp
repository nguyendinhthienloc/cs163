#include "../../header/Linked List/linked_list.h"
#include <fstream>
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
    InsertTail(value);  // Default insert is now at tail
}

void LinkedList::InsertHead(int value) {
    Node* newNode = new Node(value, { 50, 400 });
    animState = AnimState::INSERTING;
    animProgress = 0.0f;
    animNode = newNode;
    animStartPos = { 50, 200 };
    newNode->position = animStartPos;

    newNode->next = head;
    head = newNode;
    nodes.insert(nodes.begin(), newNode);
    history.push_back(Operation(Operation::Type::INSERT_HEAD, value));
}

void LinkedList::InsertTail(int value) {
    float startX = 50 + static_cast<float>(nodes.size()) * nodeSpacing;
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
    history.push_back(Operation(Operation::Type::INSERT_TAIL, value));
}

void LinkedList::InsertAfter(Node* target, int value) {
    if (!target) {
        return;
    }

    Node* newNode = new Node(value, { 0, 0 });
    newNode->next = target->next;
    target->next = newNode;

    auto it = std::find(nodes.begin(), nodes.end(), target);
    if (it != nodes.end()) {
        nodes.insert(it + 1, newNode);
    } else {
        delete newNode;
        return;
    }

    // Adjust start position for scrolling
    float adjustedTargetX = target->position.x + scrollOffsetX;
    animState = AnimState::INSERTING_AFTER;
    animProgress = 0.0f;
    animNode = newNode;
    animPrevNode = target;
    animStartPos = { adjustedTargetX + nodeSpacing, 200 };
    newNode->position = animStartPos;
    newNode->targetPosition = { adjustedTargetX + nodeSpacing, 400 };

    history.push_back(Operation(Operation::Type::INSERT_AFTER, value, newNode->position, target));
}

bool LinkedList::LoadFromFile(const char* filename) {
    std::ifstream file(filename); // Use the passed filename instead of hardcoding
    if (!file.is_open()) {
        return false;
    }

    // Clear existing list
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    nodes.clear();
    history.clear();

    bool success = false;
    int value;
    while (file >> value) {
        if (file.fail()) {
            file.clear();
            file.ignore(10000, '\n');
            continue;
        }
        if (value >= 1 && value <= 9999) {
            InsertTail(value); // Keep animation by not forcing IDLE
            success = true;
        }
    }

    file.close();
    if (success) {
        RecalculatePositions(); // Recalculate after all insertions
    }
    return success;
}

void LinkedList::Delete(int value) {
    if (!head) {
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
    if (history.empty() || IsAnimating()) return;

    Operation lastOp = history.back();
    history.pop_back();

    if (lastOp.type == Operation::Type::INSERT_HEAD) {
        Node* temp = head;
        head = head->next;
        animState = AnimState::DELETING;
        animProgress = 0.0f;
        animNode = temp;
        animStartPos = temp->position;
        nodes.erase(nodes.begin());
    } else if (lastOp.type == Operation::Type::INSERT_TAIL) {
        if (!head) return;
        Node* temp = head;
        Node* prev = nullptr;
        while (temp->next) {
            prev = temp;
            temp = temp->next;
        }
        if (prev) prev->next = nullptr;
        animState = AnimState::DELETING;
        animProgress = 0.0f;
        animNode = temp;
        animStartPos = temp->position;
        nodes.pop_back();
    } else if (lastOp.type == Operation::Type::INSERT_AFTER) {
        Node* temp = lastOp.targetNode->next;
        lastOp.targetNode->next = temp->next;
        animState = AnimState::DELETING;
        animProgress = 0.0f;
        animNode = temp;
        animStartPos = temp->position;
        auto it = std::find(nodes.begin(), nodes.end(), temp);
        if (it != nodes.end()) nodes.erase(it);
    } else if (lastOp.type == Operation::Type::DELETE) {
        Node* newNode = new Node(lastOp.value, lastOp.position);
        animState = AnimState::INSERTING;
        animProgress = 0.0f;
        animNode = newNode;
        animStartPos = { lastOp.position.x, 200 };
        newNode->position = animStartPos;
        newNode->targetPosition = lastOp.position;
        if (lastOp.prevNode) {
            newNode->next = lastOp.prevNode->next;
            lastOp.prevNode->next = newNode;
            auto it = std::find(nodes.begin(), nodes.end(), lastOp.prevNode);
            if (it != nodes.end()) nodes.insert(it + 1, newNode);
        } else {
            newNode->next = head;
            head = newNode;
            nodes.insert(nodes.begin(), newNode);
        }
    }
    RecalculatePositions();
}
