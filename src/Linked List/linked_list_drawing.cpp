#include "../../header/Linked List/linked_list.h"
#include <cmath>
#include <string>
#include <sstream>

void LinkedList::Draw() {
    float radius = 40;
    bool flash = (GetTime() - static_cast<int>(GetTime()) < 0.5f);
    float scrollX = GetScrollOffsetX();

    // Draw mode indicator in a contrasting box above pseudo-code
    std::string modeText = stepMode ? "Mode: Step by Step" : "Mode: Run at Once";
    Rectangle modeBox = { GetScreenWidth() - 400, 5, 150, 20 }; // Small box for mode text
    DrawRectangleRec(modeBox, DARKGRAY); // Dark background for contrast
    DrawRectangleLinesEx(modeBox, 1, BLACK); // Border for clarity
    DrawText(modeText.c_str(), modeBox.x + 5, modeBox.y + 2, 16, WHITE); // White text for contrast

    // Draw pseudo-code in upper right
    if (!currentPseudoCode.empty()) {
        Rectangle pseudoBox = { GetScreenWidth() - 400, 30, 380, 200 }; // Adjusted y to 30 to make space for mode box
        DrawRectangleRec(pseudoBox, Fade(LIGHTGRAY, 0.8f));
        DrawRectangleLinesEx(pseudoBox, 2, DARKGRAY);
        DrawText("Operation:", pseudoBox.x + 10, pseudoBox.y + 10, 20, BLACK);
        int yOffset = 40;
        std::string line;
        std::istringstream iss(currentPseudoCode);
        while (std::getline(iss, line)) {
            DrawText(line.c_str(), pseudoBox.x + 10, pseudoBox.y + yOffset, 16, BLACK);
            yOffset += 20;
        }
    }

    // Draw head pointer
    if (head) {
        Vector2 headPos = { head->position.x - scrollX, head->position.y - 60 };
        DrawText("head", headPos.x - 10, headPos.y - 20, 20, DARKBLUE);
        if (animState == AnimState::RELINKING && animNode && !animPrevNode) { // Insert Head
            float alpha = animProgress;
            DrawLineV(headPos, { headPos.x, head->position.y - radius }, Fade(DARKBLUE, alpha));
            Vector2 dir = { 0, radius };
            Vector2 tip = { headPos.x, head->position.y - radius };
            Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
            Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
            DrawTriangle(tip, left, right, Fade(DARKBLUE, alpha));
        } else {
            DrawLineV(headPos, { headPos.x, head->position.y - radius }, DARKBLUE);
            Vector2 dir = { 0, radius };
            Vector2 tip = { headPos.x, head->position.y - radius };
            Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
            Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
            DrawTriangle(tip, left, right, DARKBLUE);
        }
    }

    // Draw nodes in the vector
    for (auto node : nodes) {
        node->position.x = node->targetPosition.x - scrollX;
        node->position.y = node->targetPosition.y;

        std::string valueStr = std::to_string(node->value);
        if (valueStr.length() > 4) valueStr = valueStr.substr(0, 4);

        Color nodeColor = node->selected ? YELLOW : SKYBLUE;
        if (node == foundNode && animState == AnimState::IDLE) {
            nodeColor = flash ? RED : DARKPURPLE;
        } else if (node == cur && (animState == AnimState::SEARCHING || animState == AnimState::SEARCHING_FOR_DELETE)) {
            nodeColor = YELLOW;
        } else if (node == animNode && animState == AnimState::RELINKING) {
            nodeColor = RED; // Highlight node being inserted/deleted
        }

        DrawCircleV(node->position, radius, Fade(nodeColor, node->alpha));
        DrawText(valueStr.c_str(), node->position.x - 20, node->position.y - 10, 24, BLACK);

        if (node->next && animState != AnimState::RELINKING) {
            Vector2 start = CalculateArrowStart(node->position, node->next->position, radius);
            Vector2 end = CalculateArrowEnd(node->position, node->next->position, radius);
            DrawLineEx(start, end, 3.0f, BLACK);
            Vector2 dir = { end.x - start.x, end.y - start.y };
            float len = Vector2Length(dir);
            if (len > 0) {
                dir.x /= len; dir.y /= len;
                Vector2 tip = end;
                Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
                Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
                DrawTriangle(tip, left, right, BLACK);
            }
        }
    }

    // Draw relinking animation for insertion (Insert Tail or Insert After)
    if (animState == AnimState::RELINKING && animPrevNode && animNode) {
        Vector2 start = CalculateArrowStart(animPrevNode->position, animNode->position, radius);
        Vector2 end = CalculateArrowEnd(animPrevNode->position, animNode->position, radius);
        float alpha = animProgress;
        DrawLineEx(start, end, 3.0f, Fade(GREEN, alpha));
        Vector2 dir = { end.x - start.x, end.y - start.y };
        float len = Vector2Length(dir);
        if (len > 0) {
            dir.x /= len; dir.y /= len;
            Vector2 tip = end;
            Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
            Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
            DrawTriangle(tip, left, right, Fade(GREEN, alpha));
        }

        if (animNextNode) { // Insert After
            Vector2 start2 = CalculateArrowStart(animNode->position, animNextNode->position, radius);
            Vector2 end2 = CalculateArrowEnd(animNode->position, animNextNode->position, radius);
            DrawLineEx(start2, end2, 3.0f, Fade(GREEN, alpha));
            Vector2 dir2 = { end2.x - start2.x, end2.y - start2.y };
            float len2 = Vector2Length(dir2);
            if (len2 > 0) {
                dir2.x /= len2; dir2.y /= len2;
                Vector2 tip2 = end2;
                Vector2 left2 = Vector2Add(tip2, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir2) + 30 * DEG2RAD));
                Vector2 right2 = Vector2Add(tip2, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir2) - 30 * DEG2RAD));
                DrawTriangle(tip2, left2, right2, Fade(GREEN, alpha));
            }
        }
    }

    // Draw relinking animation for deletion
    if (animState == AnimState::RELINKING && animPrevNode && animNode && animNextNode && !animNextNode->next) {
        Vector2 start = CalculateArrowStart(animPrevNode->position, animNextNode->position, radius);
        Vector2 end = CalculateArrowEnd(animPrevNode->position, animNextNode->position, radius);
        float alpha = animProgress;
        DrawLineEx(start, end, 3.0f, Fade(GREEN, alpha));
        Vector2 dir = { end.x - start.x, end.y - start.y };
        float len = Vector2Length(dir);
        if (len > 0) {
            dir.x /= len; dir.y /= len;
            Vector2 tip = end;
            Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
            Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
            DrawTriangle(tip, left, right, Fade(GREEN, alpha));
        }
    }

    // Draw animNode for GENERATING/MOVING
    if ((animState == AnimState::GENERATING || animState == AnimState::MOVING || animState == AnimState::RELINKING) && animNode) {
        Vector2 animPos = { animNode->position.x - scrollX, animNode->position.y };
        std::string valueStr = std::to_string(animNode->value);
        if (valueStr.length() > 4) valueStr = valueStr.substr(0, 4);
        DrawCircleV(animPos, radius, Fade(GREEN, animNode->alpha));
        DrawText(valueStr.c_str(), animPos.x - 20, animPos.y - 10, 24, BLACK);
    }

    // Draw cur pointer
    if ((animState == AnimState::SEARCHING || animState == AnimState::SEARCHING_FOR_DELETE) && cur) {
        Vector2 curDisplayPos = { curPos.x - scrollX, curPos.y - 60 };
        DrawText("cur", curDisplayPos.x - 10, curDisplayPos.y - 20, 20, ORANGE);
        DrawLineEx(curDisplayPos, { curPos.x - scrollX, cur->position.y }, 2.0f, ORANGE);
        Vector2 dir = { 0, cur->position.y - curDisplayPos.y };
        float len = Vector2Length(dir);
        if (len > 0) {
            dir.x /= len; dir.y /= len;
            Vector2 tip = { curPos.x - scrollX, cur->position.y };
            Vector2 left = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) + 30 * DEG2RAD));
            Vector2 right = Vector2Add(tip, Vector2Rotate({-10, 0}, Vector2Angle({1, 0}, dir) - 30 * DEG2RAD));
            DrawTriangle(tip, left, right, ORANGE);
        }
    }

    // Draw NULL at the end
    if (nodes.size() > 0) {
        Vector2 lastPos = nodes.back()->position;
        Vector2 nullPos = { lastPos.x + 50, lastPos.y };
        DrawText("NULL", nullPos.x, nullPos.y - 10, 20, DARKGRAY);
        DrawLineV({lastPos.x + radius, lastPos.y}, nullPos, DARKGRAY);
    }
}

void LinkedList::RecalculatePositions() {
    float screenWidth = GetScreenWidth();
    float maxNodes = 8;
    float startX = 50;
    float yPos = 450;

    static size_t lastNodeCount = 0;
    if (nodes.size() == lastNodeCount) return;
    lastNodeCount = nodes.size();

    nodeSpacing = (nodes.size() > maxNodes) ? (screenWidth - 100) / nodes.size() : 120;
    if (nodeSpacing < 60) nodeSpacing = 60;

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