#include "../../header/Linked List/ui_linked_list.h"
#include "../../header/ui_menu.h"
#include <algorithm>
#include <string>

void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX) {
    

    bool hasNodes = list.GetSize() > 0;
    static bool undoInProgress = false;

    // Input box
    Rectangle inputBox = { 130, 20, 150, 40 };
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, BLACK);

    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputState.inputActive = true;
        //Clear the buffer
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
        Vector2 textSize = MeasureTextEx(GetFontDefault(), inputState.inputBuffer, 20, 1);
        float cursorX = inputBox.x + 10 + textSize.x;
        float cursorY = inputBox.y + 10;
        bool cursorVisible = (GetTime() - static_cast<int>(GetTime()) < 0.5f);
        if (cursorVisible) DrawLine(cursorX, cursorY, cursorX, cursorY + 20, BLACK);
    }
    DrawText(inputState.inputBuffer, inputBox.x + 10, inputBox.y + 10, 20, BLACK);
    DrawText("Value (1-9999):", 130, 70, 20, DARKGRAY);

// Node selection
if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, inputBox)) {
    bool selected = false;
    for (int i = 0; i < list.GetSize(); i++) {
        Node* node = list.GetNodeAt(i);
        if (node) {
            // Adjust node position for scrolling when comparing with mouse
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
    // Sync selected flag with selectedNode
    for (int i = 0; i < list.GetSize(); i++) {
        Node* n = list.GetNodeAt(i);
        if (n) n->selected = (n == list.GetSelectedNode());
    }
}

    // Buttons in 3 rows
    float buttonWidth = 100;
    float buttonHeight = 40;
    float buttonSpacing = 30;
    float startX = 300;
    float row1Y = 20;
    float row2Y = 70;
    float row3Y = 120;
    //Insert Head Button
    if (DrawButton({ startX, row1Y, buttonWidth, buttonHeight }, "Insert Head", LIME, inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.InsertHead(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    //Insert Tail Button
    if (DrawButton({ startX + buttonWidth + buttonSpacing + 30, row1Y, buttonWidth, buttonHeight }, "Insert Tail", GREEN, inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.InsertTail(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    
    // Insert After button
bool canInsertAfter = list.GetSize() > 0 && list.GetSelectedNode() && inputState.inputLength > 0 && !list.IsAnimating();
if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing + 30), row1Y, buttonWidth, buttonHeight }, "Insert After", YELLOW, canInsertAfter)) {
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
    //Delete button
    if (DrawButton({ startX, row2Y, buttonWidth, buttonHeight }, "Delete", RED, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Delete(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    //Search button
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row2Y, buttonWidth, buttonHeight }, "Search", BLUE, hasNodes && inputState.inputLength > 0 && !list.IsAnimating())) {
        int value;
        if (ValidateInput(inputState.inputBuffer, value)) {
            list.Search(value);
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
        }
    }
    //Load File button
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight }, "Load File", BROWN, !list.IsAnimating())) {
        if (list.LoadFromFile("list.txt")) {
            feedbackMessage = "File loaded successfully!";
        } else {
            feedbackMessage = "Failed to load file 'list.txt'!";
        }
        feedbackTimer = GetTime() + 2.0;
        inputState.inputLength = 0;
        inputState.inputBuffer[0] = '\0';
    } 
    //Clear button
    if (DrawButton({ startX, row3Y, buttonWidth, buttonHeight }, "Clear", PURPLE, true)) {
        inputState.inputLength = 0;
        inputState.inputBuffer[0] = '\0';
        while (list.GetSize() > 0) {
            list.Delete(list.GetNodeAt(0)->value);
        }
        list.SetFoundNode(nullptr);
        list.SetScrollOffsetX(0);
        undoInProgress = false;
    }
    //Undo button
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row3Y, buttonWidth, buttonHeight }, "Undo", ORANGE, !list.GetHistory().empty() && !list.IsAnimating() && !undoInProgress)) {
        list.Undo();
        undoInProgress = true;
    }
    //Random button
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row3Y, buttonWidth, buttonHeight }, "Random", PINK, !list.IsAnimating())) {
        while (list.GetSize() > 0) list.Delete(list.GetNodeAt(0)->value);
        int numNodes = GetRandomValue(3, 10);
        for (int i = 0; i < numNodes; i++) {
            list.InsertTail(GetRandomValue(1, 9999));
        }
        undoInProgress = false;
    }
    //Check if undo is complete
    if (undoInProgress && !list.IsAnimating()) {
        undoInProgress = false;
    }

    // History box
    static float historyScroll = 0.0f;
    Rectangle historyBox = { 20, 120, 200, 160 };
    DrawRectangleRec(historyBox, Fade(GRAY, 0.5f));
    DrawRectangleLinesEx(historyBox, 2, DARKGRAY);
    DrawText("History:", 25, 125, 20, BLACK);

    const std::vector<Operation>& history = list.GetHistory();
    int maxVisible = 6;
    float scrollSpeed = 20.0f;
    // Scroll history with mouse wheel
    if (CheckCollisionPointRec(mousePos, historyBox)) {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            historyScroll -= wheelMove * scrollSpeed;
            int maxScroll = std::max(0, static_cast<int>(history.size() - maxVisible)) * 20;
            historyScroll = std::max(0.0f, std::min(historyScroll, static_cast<float>(maxScroll)));
        }
    }
    // Draw history items
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
        float textY = 150 + (i * 20) - historyScroll;
        DrawText(opStr.c_str(), 25, textY, 16, BLACK);
    }
    EndScissorMode();

    // Draw feedback message
    if (feedbackTimer > GetTime()) {
        DrawText(feedbackMessage.c_str(), GetScreenWidth() / 2 - MeasureText(feedbackMessage.c_str(), 20) / 2, 50, 20, feedbackMessage[0] == 'F' ? RED : GREEN);
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