#include "ui_linked_list.h"
#include "ui_menu.h"  // For DrawButton and DrawBackButton
#include <algorithm>

void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX) {
    ClearBackground(RAYWHITE);
    DrawBackButton();

    bool hasNodes = list.GetSize() > 0;
    static bool undoInProgress = false;

    // Input box
    Rectangle inputBox = { 130, 20, 180, 40 };
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, BLACK);

    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputState.inputActive = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputState.inputActive = false;
    }

    if (inputState.inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && (inputState.inputLength < 4)) {
                inputState.inputBuffer[inputState.inputLength] = (char)key;
                inputState.inputLength++;
                inputState.inputBuffer[inputState.inputLength] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && inputState.inputLength > 0) {
            inputState.inputLength--;
            inputState.inputBuffer[inputState.inputLength] = '\0';
        }
    }

    DrawText(inputState.inputBuffer, inputBox.x + 10, inputBox.y + 10, 20, BLACK);
    if (inputState.inputActive) {
        Vector2 textSize = MeasureTextEx(GetFontDefault(), inputState.inputBuffer, 20, 1);
        float cursorX = inputBox.x + 10 + textSize.x;
        float cursorY = inputBox.y + 10;
        bool cursorVisible = (GetTime() - static_cast<int>(GetTime()) < 0.5f);
        if (cursorVisible) {
            DrawLine(cursorX, cursorY, cursorX, cursorY + 20, BLACK);
        }
    }
    DrawText("Value (1-9999):", 130, 70, 20, DARKGRAY);

    // Buttons in 2 rows
    float buttonWidth = 100;
    float buttonHeight = 40;
    float buttonSpacing = 10;
    float startX = 320;
    float row1Y = 20;
    float row2Y = 70;

    if (DrawButton({ startX, row1Y, buttonWidth, buttonHeight }, "Insert", LIME, inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Insert(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row1Y, buttonWidth, buttonHeight }, "Delete", RED, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Delete(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row1Y, buttonWidth, buttonHeight }, "Search", BLUE, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Search(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }

    if (DrawButton({ startX, row2Y, buttonWidth, buttonHeight }, "Clear", PURPLE, true)) {
        inputState.inputLength = 0;
        inputState.inputBuffer[0] = '\0';
        while (list.GetSize() > 0) {
            list.Delete(list.GetNodeAt(0)->value);
        }
        list.SetFoundNode(nullptr);
        list.SetScrollOffsetX(0);
        undoInProgress = false;
    }
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row2Y, buttonWidth, buttonHeight }, "Undo", ORANGE, !list.GetHistory().empty() && !list.IsAnimating() && !undoInProgress)) {
        list.Undo();
        undoInProgress = true;
    }
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight }, "Random", PINK, !list.IsAnimating())) {
        while (list.GetSize() > 0) list.Delete(list.GetNodeAt(0)->value);
        int numNodes = GetRandomValue(3, 10);
        for (int i = 0; i < numNodes; i++) {
            list.Insert(GetRandomValue(1, 9999));
        }
        undoInProgress = false;
    }

    if (undoInProgress && !list.IsAnimating()) {
        undoInProgress = false;
    }

    // History box
    static float historyScroll = 0.0f;
    Rectangle historyBox = { 20, 120, 200, 160 };
    DrawRectangleRec(historyBox, Fade(GRAY, 0.5f));
    DrawRectangleLinesEx(historyBox, 2, DARKGRAY);
    DrawText("History:", 25, 125, 20, BLACK);

    const auto& history = list.GetHistory();
    int maxVisible = 6;
    float scrollSpeed = 20.0f;

    if (CheckCollisionPointRec(mousePos, historyBox)) {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            historyScroll -= wheelMove * scrollSpeed;
            int maxScroll = std::max(0, (int)history.size() - maxVisible) * 20;
            historyScroll = fmaxf(0, fminf(historyScroll, maxScroll));
        }
    }

    BeginScissorMode(historyBox.x, historyBox.y + 30, historyBox.width, historyBox.height - 30);
    int startIdx = (int)(historyScroll / 20);
    for (int i = startIdx; i < (int)history.size() && (i - startIdx) < maxVisible; i++) {
        std::string opStr = (history[i].type == Operation::Type::INSERT) 
            ? "Insert " + std::to_string(history[i].value) 
            : "Delete " + std::to_string(history[i].value);
        float textY = 150 + (i * 20) - historyScroll;
        DrawText(opStr.c_str(), 25, textY, 16, BLACK);
    }
    EndScissorMode();

    // Draw the linked list
    list.Draw();

    // Handle list scrolling
    float listScrollSpeed = 30.0f;
    if (GetMouseWheelMove() != 0 && !CheckCollisionPointRec(mousePos, historyBox)) {
        float newScrollOffsetX = list.GetScrollOffsetX() - GetMouseWheelMove() * listScrollSpeed;
        float minScroll = 0;
        float maxScroll = list.GetSize() * 100 - GetScreenWidth() + 200;
        list.SetScrollOffsetX(fmaxf(minScroll, fminf(newScrollOffsetX, maxScroll)));
    }
}

bool ValidateInput(const char* input, int& value) {
    try {
        value = std::stoi(input);
        return value >= 1 && value <= 9999;
    }
    catch (...) {
        return false;
    }
}