#include "linked_list.h"
#include <cmath>

void LinkedList::Draw() {
    float radius = 30;
    bool flash = (GetTime() - static_cast<int>(GetTime()) < 0.5f);
    float scrollX = GetScrollOffsetX();

    for (auto node : nodes) {
        node->position.x = node->targetPosition.x - scrollX;
        node->position.y = node->targetPosition.y;

        std::string valueStr = std::to_string(node->value);
        if (valueStr.length() > 4) valueStr = valueStr.substr(0, 4);

        Color nodeColor = SKYBLUE;
        if (node == animNode && animState == AnimState::INSERTING) {
            nodeColor = GREEN;
        } else if (node == foundNode && animState == AnimState::IDLE) {
            nodeColor = flash ? RED : DARKPURPLE;
        } else if (node == cur && animState == AnimState::SEARCHING) {
            nodeColor = YELLOW;
        }

        DrawCircleV(node->position, radius, nodeColor);
        DrawText(valueStr.c_str(), node->position.x - 15, node->position.y - 10, 20, BLACK);

        if (node->next) {
            Vector2 startArrow = CalculateArrowStart(node->position, node->next->position, radius);
            Vector2 endArrow = CalculateArrowEnd(node->position, node->next->position, radius);
            DrawLineEx(startArrow, endArrow, 3.0f, BLACK);
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

    // Draw cur pointer during search
    if (animState == AnimState::SEARCHING && cur) {
        DrawText("cur", curPos.x - 10, curPos.y - 40, 20, ORANGE);  // Changed to ORANGE
        DrawLineV(curPos, {curPos.x, curPos.y - 20}, ORANGE);      // Changed to ORANGE
    }

    // Always draw NULL at the end if list is not empty
    if (nodes.size() > 0) {
        Vector2 lastPos = nodes.back()->position;
        DrawText("NULL", lastPos.x + 40, lastPos.y, 20, DARKGRAY);
        DrawLineV({lastPos.x + radius, lastPos.y}, {lastPos.x + 40, lastPos.y}, DARKGRAY);
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