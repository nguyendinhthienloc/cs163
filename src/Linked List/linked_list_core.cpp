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

int LinkedList::GetSize() { 
    if (nodes.size() > 1000) {
        DrawText("Error: List size exceeds safe limit!", 50, 50, 20, RED);
        return nodes.size();
    }
    return nodes.size(); 
}

void LinkedList::Insert(int value) {
    InsertTail(value);
}

void LinkedList::InsertHead(int value) {
    if (GetSize() >= 1000) return;
    
    Node* newNode = new Node(value, { 0, 0 });
    newNode->next = head;
    head = newNode;
    nodes.insert(nodes.begin(), newNode);
    
    RecalculatePositions();
    float initialX = nodes.size() == 1 ? 50 : 50 - nodeSpacing;
    animState = AnimState::GENERATING;
    animProgress = 0.0f;
    animNode = newNode;
    animStartPos = { initialX, 550 };
    newNode->position = animStartPos;
    newNode->targetPosition = { 50, 450 };

    history.push_back(Operation(Operation::Type::INSERT_HEAD, value));
}

void LinkedList::InsertTail(int value) {
    if (GetSize() >= 1000) return;
    
    Node* newNode = new Node(value, { 0, 0 });
    if (!head) {
        head = newNode;
        cur = head;
        nodes.push_back(newNode);
    } else {
        Node* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
        cur = temp;
        animPrevNode = temp;
        nodes.push_back(newNode);
    }
    
    RecalculatePositions();
    float startX = nodes.size() == 1 ? 50 : 50 + static_cast<float>(nodes.size() - 1) * nodeSpacing;
    animState = AnimState::GENERATING;
    animProgress = 0.0f;
    animNodeReturns:
    animNode = newNode;
    animStartPos = { startX, 550 };
    newNode->position = animStartPos;
    newNode->targetPosition = { 50 + static_cast<float>(nodes.size() - 1) * nodeSpacing, 450 };

    history.push_back(Operation(Operation::Type::INSERT_TAIL, value));
}

void LinkedList::InsertAfter(Node* target, int value) {
    if (GetSize() >= 1000 || !target) return;

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

    RecalculatePositions();
    float adjustedTargetX = target->targetPosition.x + nodeSpacing;
    animState = AnimState::GENERATING;
    animProgress = 0.0f;
    animNode = newNode;
    animPrevNode = target;
    animNextNode = newNode->next;
    animStartPos = { adjustedTargetX, 550 };
    newNode->position = animStartPos;
    newNode->targetPosition = { target->targetPosition.x + nodeSpacing, 450 };

    history.push_back(Operation(Operation::Type::INSERT_AFTER, value, newNode->position, target));
}

bool LinkedList::LoadFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    nodes.clear();
    history.clear();

    bool success = false;
    int value;
    int count = 0;
    while (file >> value) {
        if (file.fail()) {
            file.clear();
            file.ignore(10000, '\n');
            continue;
        }
        if (value >= 1 && value <= 9999 && count < 1000) {
            InsertTail(value);
            success = true;
            count++;
        }
    }

    file.close();
    if (success) {
        RecalculatePositions();
    }
    return success;
}

void LinkedList::Delete(int value) {
    if (!head) {
        return;
    }

    Node* temp = head;
    Node* prev = nullptr;

    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) {
        return;
    }

    animState = AnimState::SEARCHING_FOR_DELETE;
    animProgress = 0.0f;
    animNode = temp;
    animPrevNode = prev;
    animStartPos = temp->position;
    cur = temp;

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
        if (!current) return nullptr;
        current = current->next;
    }
    return current;
}

void LinkedList::Undo() {
    if (history.empty() || IsAnimating()) return;

    Operation lastOp = history.back();
    history.pop_back();

    isUndoing = true;
    undoOperationType = lastOp.type;

    if (lastOp.type == Operation::Type::INSERT_HEAD) {
        Node* temp = head;
        if (!temp) return;
        head = head->next;
        animState = AnimState::DELETING;
        animProgress = 0.0f;
        animNode = temp;
        animStartPos = temp->position;
        auto it = std::find(nodes.begin(), nodes.end(), temp);
        if (it != nodes.end()) nodes.erase(it);
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
        auto it = std::find(nodes.begin(), nodes.end(), temp);
        if (it != nodes.end()) nodes.erase(it);
    } else if (lastOp.type == Operation::Type::INSERT_AFTER) {
        if (!lastOp.targetNode) return;
        Node* temp = lastOp.targetNode->next;
        if (!temp) return;
        lastOp.targetNode->next = temp->next;
        animState = AnimState::DELETING;
        animProgress = 0.0f;
        animNode = temp;
        animStartPos = temp->position;
        auto it = std::find(nodes.begin(), nodes.end(), temp);
        if (it != nodes.end()) nodes.erase(it);
    } else if (lastOp.type == Operation::Type::DELETE) {
        if (GetSize() >= 1000) return;
        Node* newNode = new Node(lastOp.value, lastOp.position);
        animState = AnimState::GENERATING;
        animProgress = 0.0f;
        animNode = newNode;
        animStartPos = { lastOp.position.x, 550 };
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

void LinkedList::ResetAnimationState() {
    animState = AnimState::IDLE;
    animProgress = 0.0f;
    if (animNode) {
        auto it = std::find(nodes.begin(), nodes.end(), animNode);
        if (it == nodes.end()) {
            delete animNode;
        }
        animNode = nullptr;
    }
    animPrevNode = nullptr;
    animNextNode = nullptr;
    cur = nullptr;
    foundNode = nullptr;
    currentPseudoCode = "";
    stepPaused = false;
    isUndoing = false;
}

Node* LinkedList::GetHead() {
    return head;
}

void LinkedList::SetHead(Node* newHead) {
    head = newHead;
}

std::vector<Node*>& LinkedList::GetNodes() {
    return nodes;
}

void LinkedList::ClearHistory() {
    history.clear();
}