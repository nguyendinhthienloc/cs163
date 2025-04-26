#include "../../header/Linked List/ui_linked_list.h"
#include "../../header/ui_menu.h"
#include "../../resource/tinyfiledialogs.h"
#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>

void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX) {
    bool hasNodes = list.GetSize() > 0;
    static bool undoInProgress = false;

    // Input box
    Rectangle inputBox = { 150, 30, 180, 50 };
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, BLACK);

    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputState.inputActive = true;
        memset(inputState.inputBuffer, 0, sizeof(inputState.inputBuffer));
        inputState.inputLength = 0;
    } 
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputState.inputActive = false;
    }

    // Input handling
    if (inputState.inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && (inputState.inputLength < 4)) {
                inputState.inputBuffer[inputState.inputLength] = static_cast<char>(key);
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

    // Draw input text
    if (inputState.inputActive) {
        Vector2 textSize = MeasureTextEx(GetFontDefault(), inputState.inputBuffer, 24, 1);
        float cursorX = inputBox.x + 15 + textSize.x;
        float cursorY = inputBox.y + 15;
        bool cursorVisible = (GetTime() - static_cast<int>(GetTime()) < 0.5f);
        if (cursorVisible) DrawLine(cursorX, cursorY, cursorX, cursorY + 24, BLACK);
    }
    DrawText(inputState.inputBuffer, inputBox.x + 15, inputBox.y + 15, 24, BLACK);
    DrawText("Value (1-9999):", 150, 90, 24, DARKGRAY);

    // Node selection
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, inputBox)) {
        bool selected = false;
        for (int i = 0; i < list.GetSize(); i++) {
            Node* node = list.GetNodeAt(i);
            if (node) {
                Vector2 adjustedNodePos = { node->position.x + list.GetScrollOffsetX(), node->position.y };
                if (Vector2Distance(mousePos, adjustedNodePos) < 40) {
                    list.SetSelectedNode(node);
                    selected = true;
                    break;
                }
            }
        }
        if (!selected) {
            list.SetSelectedNode(nullptr);
        }
        for (int i = 0; i < list.GetSize(); i++) {
            Node* n = list.GetNodeAt(i);
            if (n) n->selected = (n == list.GetSelectedNode());
        }
    }

    // Buttons in 4 rows
    float buttonWidth = 140;
    float buttonHeight = 50;
    float buttonSpacing = 20;
    float startX = 350;
    float row1Y = 30;
    float row2Y = 100;
    float row3Y = 170;
    float row4Y = 240;

    // Insert Head Button
    if (DrawButton({ startX, row1Y, buttonWidth, buttonHeight }, "Insert Head", LIME, inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.InsertHead(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }

    // Insert Tail Button
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row1Y, buttonWidth, buttonHeight}, "Insert Tail", LIME, inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.InsertTail(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }

    // Insert After button
    bool canInsertAfter = list.GetSize() > 0 && list.GetSelectedNode() && inputState.inputLength > 0 && !list.IsAnimating();
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row1Y, buttonWidth, buttonHeight }, "Insert After", LIME, canInsertAfter)) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            Node* selected = list.GetSelectedNode();
            if (selected) {
                list.InsertAfter(selected, value);
                feedbackMessage = "Inserted " + std::to_string(value) + " after " + std::to_string(selected->value);
                feedbackTimer = GetTime() + 2.0;
            } else {
                feedbackMessage = "Error: No node selected!";
                feedbackTimer = GetTime() + 2.0;
            }
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        } else {
            feedbackMessage = "Invalid input: " + std::string(inputState.inputBuffer);
            feedbackTimer = GetTime() + 2.0;
        }
    }

    // Delete button
    if (DrawButton({ startX, row2Y, buttonWidth, buttonHeight }, "Delete", RED, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Delete(value);
            feedbackMessage = "Deleting " + std::to_string(value);
            feedbackTimer = GetTime() + 2.0;
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        } else {
            feedbackMessage = "Invalid input: " + std::string(inputState.inputBuffer);
            feedbackTimer = GetTime() + 2.0;
        }
    }

    // Search button
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row2Y, buttonWidth, buttonHeight }, "Search", BLUE, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Search(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }

    // Load File button
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight }, "Load File", BROWN, !list.IsAnimating())) {
        const char *filters[] = { "*.txt" };
        const char *filename = tinyfd_openFileDialog("Select a Text File", "", 1, filters, "Text Files", 0);
        if (filename) {
            if (list.LoadFromFile(filename)) {
                feedbackMessage = "File loaded successfully!";
            } else {
                feedbackMessage = "Failed to load file!";
            }
        } else {
            feedbackMessage = "No file selected!";
        }
        feedbackTimer = GetTime() + 2.0;
        inputState.inputLength = 0;
        inputState.inputBuffer[0] = '\0';
    } 

    // Clear button
    if (DrawButton({ startX, row3Y, buttonWidth, buttonHeight }, "Clear", RED, true)) {
        // Stop any ongoing animations
        list.ResetAnimationState();
        // Directly clear the list without animations
        while (list.GetSize() > 0) {
            Node* temp = list.GetHead();
            if (!temp) break;
            list.SetHead(temp->next);
            auto it = std::find(list.GetNodes().begin(), list.GetNodes().end(), temp);
            if (it != list.GetNodes().end()) {
                list.GetNodes().erase(it);
            }
            delete temp;
        }
        list.SetFoundNode(nullptr);
        list.SetScrollOffsetX(0);
        list.ClearHistory();
        inputState.inputLength = 0;
        inputState.inputBuffer[0] = '\0';
        undoInProgress = false;
        feedbackMessage = "List cleared!";
        feedbackTimer = GetTime() + 2.0;
    }

    // Undo button
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row3Y, buttonWidth, buttonHeight }, "Undo", ORANGE, !list.GetHistory().empty() && !list.IsAnimating() && !undoInProgress)) {
        list.Undo();
        undoInProgress = true;
    }

    // Random button
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row3Y, buttonWidth, buttonHeight }, "Random", PINK, !list.IsAnimating())) {
        list.ResetAnimationState();
        while (list.GetSize() > 0) {
            Node* temp = list.GetHead();
            if (!temp) break;
            list.SetHead(temp->next);
            auto it = std::find(list.GetNodes().begin(), list.GetNodes().end(), temp);
            if (it != list.GetNodes().end()) {
                list.GetNodes().erase(it);
            }
            delete temp;
        }
        int numNodes = GetRandomValue(3, 10);
        for (int i = 0; i < numNodes; i++) {
            list.InsertTail(GetRandomValue(1, 9999));
        }
        list.ClearHistory();
        undoInProgress = false;
    }

    // Animation control buttons
    bool isStepMode = list.IsStepMode();
    if (DrawButton({ startX, row4Y, buttonWidth, buttonHeight }, isStepMode ? "Run Mode" : "Step Mode", PURPLE, true)) {
        list.ToggleStepMode();
    }

    if (DrawButton({ startX + buttonWidth + buttonSpacing, row4Y, buttonWidth, buttonHeight }, "Next Step", PURPLE, isStepMode && list.IsAnimating())) {
        list.NextStep();
    }

    // Check if undo is complete
    if (undoInProgress && !list.IsAnimating()) {
        undoInProgress = false;
    }

    // History box
    static float historyScroll = 0.0f;
    Rectangle historyBox = { 20, 150, 240, 200 };
    DrawRectangleRec(historyBox, Fade(GRAY, 0.5f));
    DrawRectangleLinesEx(historyBox, 2, DARKGRAY);
    DrawText("History:", 25, 155, 24, BLACK);

    const std::vector<Operation>& history = list.GetHistory();
    int maxVisible = 7;
    float scrollSpeed = 20.0f;
    if (CheckCollisionPointRec(mousePos, historyBox)) {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            historyScroll -= wheelMove * scrollSpeed;
            int maxScroll = std::max(0, static_cast<int>(history.size() - maxVisible)) * 20;
            historyScroll = std::max(0.0f, std::min(historyScroll, static_cast<float>(maxScroll)));
        }
    }

    BeginScissorMode(historyBox.x, historyBox.y + 30, historyBox.width, historyBox.height - 30);
    int startIdx = static_cast<int>(historyScroll / 20);
    for (int i = startIdx; i < static_cast<int>(history.size()) && (i - startIdx) < maxVisible; i++) {
        std::string opStr;
        switch (history[i].type) {
            case Operation::Type::INSERT: opStr = "Insert " + std::to_string(history[i].value); break;
            case Operation::Type::INSERT_HEAD: opStr = "InsHead " + std::to_string(history[i].value); break;
            case Operation::Type::INSERT_TAIL: opStr = "InsTail " + std::to_string(history[i].value); break;
            case Operation::Type::INSERT_AFTER: opStr = "InsAfter " + std::to_string(history[i].value); break;
            case Operation::Type::DELETE: opStr = "Delete " + std::to_string(history[i].value); break;
        }
        float textY = 180 + (i * 20) - historyScroll;
        DrawText(opStr.c_str(), 25, textY, 18, BLACK);
    }
    EndScissorMode();

    // Draw feedback message
    if (feedbackTimer > GetTime()) {
        DrawText(feedbackMessage.c_str(), GetScreenWidth() / 2 - MeasureText(feedbackMessage.c_str(), 24) / 2, 60, 24, feedbackMessage[0] == 'F' ? RED : GREEN);
    }

    // Draw the linked list
    list.Draw();

    // Handle list scrolling
    float listScrollSpeed = 30.0f;
    if (GetMouseWheelMove() != 0 && !CheckCollisionPointRec(mousePos, historyBox)) {
        float newScrollOffsetX = list.GetScrollOffsetX() - GetMouseWheelMove() * listScrollSpeed;
        float minScroll = 0;
        float maxScroll = std::max(0.0f, static_cast<float>(list.GetSize() * 100 - GetScreenWidth() + 200));
        list.SetScrollOffsetX(std::max(minScroll, std::min(newScrollOffsetX, maxScroll)));
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