#include "../../header/Linked List/linked_list.h"
#include <algorithm>

void LinkedList::UpdateAnimation(float deltaTime) {
    const float TRAVERSAL_SPEED = 0.1f;
    const float ANIM_DURATION = 0.5f;
    if (animState != AnimState::IDLE) {
        animProgress += deltaTime / ANIM_DURATION;
    }

    static float notFoundTimer = 0;

    if (animState == AnimState::INSERTING && animNode) {
        animNode->position.x = Lerp(animStartPos.x, animNode->targetPosition.x, animProgress);
        animNode->position.y = Lerp(animStartPos.y, animNode->targetPosition.y, animProgress);
        if (animProgress >= 1.0f) {
            animNode->position = animNode->targetPosition;
            animState = AnimState::IDLE;
            animNode = nullptr;
            RecalculatePositions();
            animProgress = 0.0f;
        }
    } else if (animState == AnimState::INSERTING_AFTER && animNode && animPrevNode) {
        animNode->position.x = Lerp(animStartPos.x, animNode->targetPosition.x, animProgress);
        animNode->position.y = Lerp(animStartPos.y, animNode->targetPosition.y, animProgress);
        if (animProgress >= 1.0f) {
            animNode->position = animNode->targetPosition;
            animState = AnimState::IDLE;
            animNode = nullptr;
            animPrevNode = nullptr;
            RecalculatePositions(); // Ensure all positions are updated
            animProgress = 0.0f;
        }
    } else if (animState == AnimState::DELETING && animNode) { 
        animNode->alpha = 1.0f - animProgress;
        animNode->position.y = animStartPos.y + (animProgress * 100);
        if (animProgress >= 1.0f) {
            delete animNode;
            animNode = nullptr;
            animState = AnimState::IDLE;
            RecalculatePositions();
            animProgress = 0.0f;
        }
    } else if (animState == AnimState::SEARCHING && cur) {
        // Search animation
        if (cur->value == animNode->value) {
            foundNode = cur;
            animState = AnimState::IDLE;
            animProgress = 0.0f;
            highlightTimer = GetTime() + 2.0f;
            delete animNode;
            animNode = nullptr;
        } else if (cur->next) {
            Vector2 target = cur->next->position;
            curPos.x += (target.x - curPos.x) * TRAVERSAL_SPEED;
            curPos.y += (target.y - curPos.y) * TRAVERSAL_SPEED;
            if (Vector2Distance(curPos, target) < 5.0f) {
                cur = cur->next;
                curPos = cur->position;
            }
        } else {
            animState = AnimState::IDLE;
            animProgress = 0.0f;
            delete animNode;
            animNode = nullptr;
            notFoundTimer = GetTime() + 1.0f;
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