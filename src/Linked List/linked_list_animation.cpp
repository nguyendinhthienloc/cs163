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

    if (animState == AnimState::INSERTING && animNode) {
        SetPseudoCode("Creating and positioning new node");
        animNode->position.x = Lerp(animStartPos.x, animNode->targetPosition.x, animProgress);
        animNode->position.y = Lerp(animStartPos.y, animNode->targetPosition.y, animProgress);
        if (animProgress >= 1.0f) {
            animNode->position = animNode->targetPosition;
            animState = AnimState::IDLE;
            animNode = nullptr;
            RecalculatePositions();
            animProgress = 0.0f;
            currentPseudoCode = "";
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::INSERTING_AFTER && animNode && animPrevNode) {
        SetPseudoCode("Creating and positioning new node after target");
        animNode->position.x = Lerp(animStartPos.x, animNode->targetPosition.x, animProgress);
        animNode->position.y = Lerp(animStartPos.y, animNode->targetPosition.y, animProgress);
        if (animProgress >= 1.0f) {
            animNode->position = animNode->targetPosition;
            animState = AnimState::IDLE;
            animNode = nullptr;
            animPrevNode = nullptr;
            RecalculatePositions();
            animProgress = 0.0f;
            currentPseudoCode = "";
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::SEARCHING_FOR_DELETE && animNode) {
        SetPseudoCode("Preparing to delete node");
        animState = AnimState::RELINKING;
        animProgress = 0.0f;
        animNextNode = animNode->next;
        foundNode = animNode;
        if (stepMode) stepPaused = true;
    } else if (animState == AnimState::RELINKING && animNode) {
        SetPseudoCode("Relinking pointers");
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
                // Log error if node not found in vector
                DrawText("Error: Node not found in vector during relinking!", 50, 50, 20, RED);
            }
            animState = AnimState::DELETING;
            animProgress = 0.0f;
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::DELETING && animNode) {
        SetPseudoCode("Removing node");
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
            if (stepMode) stepPaused = true;
        }
    } else if (animState == AnimState::SEARCHING && cur) {
        SetPseudoCode("Searching for value");
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