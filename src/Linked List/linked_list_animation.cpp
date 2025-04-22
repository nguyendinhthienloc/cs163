#include "../../header/Linked List/linked_list.h"
#include <algorithm>

void LinkedList::SetPseudoCode(const std::string& code) {
    currentPseudoCode = code;
}

void LinkedList::UpdateAnimation(float deltaTime) {
    const float TRAVERSAL_SPEED = stepMode ? 0.05f : 0.15f;
    const float ANIM_DURATION = stepMode ? 0.5f : 0.5f;
    const float RELINK_DURATION = stepMode ? 0.5f : 0.5f;
    static float notFoundTimer = 0;

    if (stepMode && stepPaused && animState != AnimState::IDLE) {
        return;
    }

    if (animState != AnimState::IDLE) {
        animProgress += deltaTime / (animState == AnimState::RELINKING ? RELINK_DURATION : ANIM_DURATION);
    }

    if (animState == AnimState::GENERATING && animNode) {
        // Step 1: Node generation
        if (isUndoing && undoOperationType == Operation::Type::DELETE) {
            SetPseudoCode("Undo Delete:\nStep 1: Create newNode = new Node(value)");
        } else {
            SetPseudoCode("Insert:\nStep 1: Create newNode = new Node(value)");
        }
        animNode->alpha = animProgress; // Fade in effect
        if (animProgress >= 1.0f) {
            animNode->alpha = 1.0f;
            animState = AnimState::MOVING;
            animProgress = 0.0f;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::MOVING && animNode) {
        // Step 2: Move node to target position
        if (isUndoing && undoOperationType == Operation::Type::DELETE) {
            SetPseudoCode("Undo Delete:\nStep 2: Move newNode to position");
        } else {
            SetPseudoCode("Insert:\nStep 2: Move newNode to position");
        }
        animNode->position.x = Lerp(animStartPos.x, animNode->targetPosition.x, animProgress);
        animNode->position.y = Lerp(animStartPos.y, animNode->targetPosition.y, animProgress);
        if (animProgress >= 1.0f) {
            animNode->position = animNode->targetPosition;
            animState = AnimState::RELINKING;
            animProgress = 0.0f;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::RELINKING && animNode) {
        // Step 3: Relink arrows (same state used for both insertion and deletion)
        if (isUndoing && undoOperationType == Operation::Type::DELETE) {
            if (undoOperationType == Operation::Type::DELETE) {
                SetPseudoCode("Undo Delete:\nStep 3: newNode->next = prev->next\nprev->next = newNode");
            }
        } else {
            if (animPrevNode) { // Insert After or Insert Tail
                SetPseudoCode("Insert:\nStep 3: target->next = newNode\nnewNode->next = nextNode");
            } else { // Insert Head
                SetPseudoCode("Insert:\nStep 3: newNode->next = head\nhead = newNode");
            }
        }

        if (animProgress >= 1.0f) {
            if (animPrevNode && animNextNode) { // Insert After
                // Already linked in InsertAfter
            } else if (animPrevNode) { // Insert Tail
                // Already linked in InsertTail
            } else { // Insert Head
                // Already linked in InsertHead
            }
            animState = AnimState::IDLE;
            animNode = nullptr;
            animPrevNode = nullptr;
            animNextNode = nullptr;
            RecalculatePositions();
            animProgress = 0.0f;
            currentPseudoCode = "";
            isUndoing = false;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::SEARCHING_FOR_DELETE && animNode) {
        SetPseudoCode("Delete:\ncurrent = head\nwhile current->value != value:\n    current = current->next");
        animState = AnimState::RELINKING;
        animProgress = 0.0f;
        animNextNode = animNode->next;
        foundNode = animNode;
        if (stepMode) stepPaused = true;
    } else if (animState == AnimState::RELINKING && animNode && !animNextNode) { // For deletion
        SetPseudoCode("Delete:\nif prev:\n    prev->next = current->next\nelse:\n    head = current->next");
        if (animProgress >= 1.0f) {
            if (animPrevNode) {
                animPrevNode->next = animNextNode;
            } else if (animNode == head) {
                head = animNextNode;
            }
            auto it = std::find(nodes.begin(), nodes.end(), animNode);
            if (it != nodes.end()) {
                nodes.erase(it);
            } else {
                DrawText("Error: Node not found in vector during relinking!", 50, 50, 20, RED);
            }
            animState = AnimState::DELETING;
            animProgress = 0.0f;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::DELETING && animNode) {
        if (isUndoing && (undoOperationType == Operation::Type::INSERT_HEAD || undoOperationType == Operation::Type::INSERT_TAIL || undoOperationType == Operation::Type::INSERT_AFTER)) {
            SetPseudoCode("Undo Insert:\nif prev:\n    prev->next = nullptr\nelse:\n    head = nullptr");
        } else {
            SetPseudoCode("Delete:\ndelete current");
        }
        animNode->alpha = 1.0f - animProgress;
        animNode->position.y = animStartPos.y + (animProgress * 100);
        if (animProgress >= 1.0f) {
            delete animNode;
            animNode = nullptr;
            animState = AnimState::IDLE;
            animNextNode = nullptr;
            animPrevNode = nullptr;
            cur = nullptr;
            RecalculatePositions();
            animProgress = 0.0f;
            currentPseudoCode = "";
            isUndoing = false;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::SEARCHING && cur) {
        SetPseudoCode("Search:\ncurrent = head\nwhile current:\n    if current->value == value:\n        return current\n    current = current->next");
        if (cur->next) {
            Vector2 target = cur->next->position;
            curPos.x += (target.x - curPos.x) * TRAVERSAL_SPEED;
            curPos.y += (target.y - curPos.y) * TRAVERSAL_SPEED;
            if (Vector2Distance(curPos, target) < 5.0f) {
                cur = cur->next;
                curPos = cur->position;
                if (cur->value == animNode->value) {
                    foundNode = cur;
                    animState = AnimState::IDLE;
                    animProgress = 0.0f;
                    highlightTimer = GetTime() + 2.0f;
                    delete animNode;
                    animNode = nullptr;
                    currentPseudoCode = "";
                    if (stepMode) stepPaused = true;
                } else if (!cur->next) {
                    animState = AnimState::IDLE;
                    animProgress = 0.0f;
                    delete animNode;
                    animNode = nullptr;
                    notFoundTimer = GetTime() + 1.0f;
                    currentPseudoCode = "";
                    if (stepMode) stepPaused = true;
                } else if (stepMode) {
                    stepPaused = true;
                }
            }
        } else {
            animState = AnimState::IDLE;
            animProgress = 0.0f;
            delete animNode;
            animNode = nullptr;
            notFoundTimer = GetTime() + 1.0f;
            currentPseudoCode = "";
            if (stepMode) stepPaused = true;
        }
    }

    if (foundNode && GetTime() >= highlightTimer) {
        foundNode = nullptr;
        highlightTimer = 0.0f;
    }

    if (searchAttempted && foundNode == nullptr && animState == AnimState::IDLE && GetTime() < notFoundTimer) {
        DrawText("Value not found!", GetScreenWidth() / 2 - 80, 50, 24, RED);
    } else if (GetTime() >= notFoundTimer) {
        searchAttempted = false;
        notFoundTimer = 0;
    }
}