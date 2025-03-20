#include "raylib.h"
#include "linked_list.h"
#include <iostream>
#include <algorithm>
#include <vector>


// Utility functions (unchanged)
float Vector2Distance(Vector2 v1, Vector2 v2) {
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y));
}

Vector2 Vector2Add(Vector2 v1, Vector2 v2) {
    return { v1.x + v2.x, v1.y + v2.y };
}

Vector2 Vector2Rotate(Vector2 v, float angle) {
    float rad = angle * DEG2RAD;
    return { v.x * cosf(rad) - v.y * sinf(rad), v.x * sinf(rad) + v.y * cosf(rad) };
}

float Vector2Length(Vector2 vec) {
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

LinkedList::LinkedList() : head(nullptr), foundNode(nullptr), cur(nullptr), scrollOffsetX(0), scrollOffsetY(0), animStartPos{ 0, 0 } {}

LinkedList::~LinkedList() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    nodes.clear();
}

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
    }
    else {
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
        return;  // 🔴 Prevents crash if the list is empty
    }

    Node* temp = head;
    Node* prev = nullptr;
    cur = head;

    // 🔎 Search for the node to delete
    while (temp && temp->value != value) {
        prev = temp;
        temp = temp->next;
    }

    // 🚨 **If Node Not Found, Do Nothing**
    if (!temp) {
        std::cout << "Value " << value << " not found for deletion!" << std::endl;
        return;
    }

    // 🟢 **Node Found → Begin Deletion Animation**
    animState = AnimState::DELETING;
    animProgress = 0.0f;
    animNode = temp;
    animStartPos = temp->position;

    // 📌 **Update Linked List Pointers Safely**
    if (prev) {
        prev->next = temp->next;  // Remove the node from the chain
    } else {
        head = temp->next;  // If head is deleted, move head to the next node
    }

    // ✅ **Remove from `nodes` vector safely**
    auto it = std::find(nodes.begin(), nodes.end(), temp);
    if (it != nodes.end()) {
        nodes.erase(it);
    }

    cur = nullptr;  // Reset cursor pointer to prevent issues
}




void LinkedList::UpdateAnimation(float deltaTime) {
    const float ANIM_DURATION = 0.5f;
    animProgress += deltaTime / ANIM_DURATION;

    static float foundNodeTimer = 0;
    static float notFoundTimer = 0;

    if (animState == AnimState::INSERTING && animNode) {
        animNode->position.x = animStartPos.x + (animProgress * (animNode->targetPosition.x - animStartPos.x));
        animNode->position.y = animStartPos.y + (animProgress * (animNode->targetPosition.y - animStartPos.y));
        if (animProgress >= 1.0f) {
            animState = AnimState::IDLE;
            RecalculatePositions();
        }
    } 
    else if (animState == AnimState::DELETING && animNode) {
        animNode->alpha = 1.0f - animProgress;
        animNode->position.y = animStartPos.y + (animProgress * 100);

        if (animProgress >= 1.0f) {
            if (animNode) {
                delete animNode;
                animNode = nullptr;
            }
            animState = AnimState::IDLE;
            RecalculatePositions();
        }
    } 
    else if (animState == AnimState::SEARCHING && cur) {
        if (cur->value == animNode->value || animProgress >= 1.0f) {
            foundNode = cur;
            animState = AnimState::IDLE;
            animProgress = 0.0f;
            foundNodeTimer = GetTime() + 1.0f;
        }        
        else if (cur->next) {
            Vector2 target = cur->next->position;
            cur->position.x += (target.x - cur->position.x) * 0.1f;
            cur->position.y += (target.y - cur->position.y) * 0.1f;
            if (Vector2Distance(cur->position, target) < 5.0f) cur = cur->next;
        }
    }

    if (foundNode && GetTime() >= foundNodeTimer) {
        foundNode = nullptr;
    }

    if (searchAttempted && foundNode == nullptr && animState == AnimState::IDLE) {
        if (notFoundTimer == 0) {
            notFoundTimer = GetTime() + 1.0f;
        }
        if (GetTime() < notFoundTimer) {
            DrawText("Value not found!", GetScreenWidth() / 2 - 80, 50, 24, RED);
        } else {
            searchAttempted = false;
            notFoundTimer = 0;
        }
    }
}




bool LinkedList::Search(int value) {
    foundNode = nullptr;  // Reset foundNode
    cur = head;           // Start search from head every time
    animState = AnimState::SEARCHING;
    animProgress = 0.0f;
    searchAttempted = true;

    while (cur) {
        if (cur->value == value) {
            foundNode = cur;  // Found node
            animState = AnimState::IDLE;  // Stop searching after finding
            animProgress = 0.0f;
            return true;
        }
        cur = cur->next;
    }

    animState = AnimState::IDLE;  // Stop searching when not found
    return false;  // Not found
}




void LinkedList::Update(int oldValue, int newValue) {
    for (auto node : nodes) {
        if (node->value == oldValue) {
            node->value = newValue;
            break;
        }
    }
}

void LinkedList::RecalculatePositions() {
    float screenWidth = GetScreenWidth();
    float maxNodes = 8;
    float startX = 120;
    float yPos = 400;

    static size_t lastNodeCount = 0;
    if (nodes.size() == lastNodeCount) return;
    lastNodeCount = nodes.size();

    nodeSpacing = (nodes.size() > maxNodes) ? (screenWidth - 240) / nodes.size() : 100;
    if (nodeSpacing < 50) nodeSpacing = 50;

    for (auto node : nodes) {
        node->targetPosition = { startX, yPos };
        startX += nodeSpacing;
    }
}

Vector2 LinkedList::CalculateArrowStart(Vector2 from, Vector2 to, float radius) {
    Vector2 direction = { to.x - from.x, to.y - from.y };
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length == 0) return from;
    direction.x /= length;
    direction.y /= length;
    return { from.x + direction.x * radius, from.y + direction.y * radius };
}

Vector2 LinkedList::CalculateArrowEnd(Vector2 from, Vector2 to, float radius) {
    Vector2 direction = { to.x - from.x, to.y - from.y };
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length == 0) return to;
    direction.x /= length;
    direction.y /= length;
    return { to.x - direction.x * radius, to.y - direction.y * radius };
}

void LinkedList::Draw() {
    float radius = 30;  // Node radius
    float lerpSpeed = 0.1f;  // Controls smoothness of movement
    bool flash = (GetTime() - static_cast<int>(GetTime()) < 0.5f);  // Flash effect for found node

    float scrollX = GetScrollOffsetX();  // ✅ Apply horizontal scrolling

    for (auto node : nodes) {
        // ✅ Apply scrolling offset
        node->position.x = node->targetPosition.x - scrollX;
        node->position.y = node->targetPosition.y;

        // Limit value display length
        std::string valueStr = std::to_string(node->value);
        if (valueStr.length() > 4) valueStr = valueStr.substr(0, 4);

        // Default node color
        Color nodeColor = SKYBLUE;

        // Highlight conditions
        if (node == animNode && animState == AnimState::INSERTING) {
            nodeColor = GREEN;  // New node insertion
        } 
        else if (node == foundNode) {
            nodeColor = flash ? RED : DARKPURPLE;  // Blink found node
        } 
        else if (node == cur && animState == AnimState::SEARCHING) {
            nodeColor = YELLOW;  // Current node in search
        }

        // Draw the node circle
        DrawCircleV(node->position, radius, nodeColor);
        DrawText(valueStr.c_str(), node->position.x - 15, node->position.y - 10, 20, BLACK);

        // Draw arrows between linked nodes
        if (node->next) {
            Vector2 startArrow = CalculateArrowStart(node->position, node->next->position, radius);
            Vector2 endArrow = CalculateArrowEnd(node->position, node->next->position, radius);
            DrawLineEx(startArrow, endArrow, 3.0f, BLACK);

            // Draw arrowhead
            Vector2 dir = { endArrow.x - startArrow.x, endArrow.y - startArrow.y };
            float len = Vector2Length(dir);
            if (len > 0) {
                dir.x *= 10.0f / len;
                dir.y *= 10.0f / len;
                DrawTriangle(endArrow,
                    Vector2Add(endArrow, Vector2Rotate(dir, -30)),
                    Vector2Add(endArrow, Vector2Rotate(dir, 30)),
                    BLACK);
            }
        }
    }

    // Draw "Value Not Found" message temporarily
    static float notFoundTimer = 0;
    if (searchAttempted && foundNode == nullptr && animState == AnimState::IDLE) {
        if (notFoundTimer == 0) {
            notFoundTimer = GetTime() + 1.0f;  // Show message for 1 second
        }
        if (GetTime() < notFoundTimer) {
            DrawText("Value not found!", 20, 80, 24, RED);
        } else {
            searchAttempted = false;  // Reset after timeout
            notFoundTimer = 0;
        }
    }
}


Node* LinkedList::GetNodeAt(int index) {
    if (index < 0 || index >= GetSize()) return nullptr; // Ensure index is valid
    Node* current = head; // Assuming 'head' is the start of the linked list
    for (int i = 0; i < index; ++i) {
        current = current->next; // Traverse to the desired node
    }
    return current;
}