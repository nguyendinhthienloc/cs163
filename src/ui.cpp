#include "raylib.h"
#include "linked_list.h"    // Include linked list header file
#include "ui.h"
#include <cstring>
#include <algorithm> 

// Removed the redundant enum definition as it is already in ui.h
// Screen enum is already declared in ui.h

Screen currentScreen = MENU;
float transitionAlpha = 0.0f;
bool transitioning = false;
Screen nextScreen = MENU;

bool DrawButton(Rectangle rect, const char* text, Color color, bool enabled) {
    if (!enabled) color = Fade(color, 0.3f);
    Vector2 mouse = GetMousePosition();
    bool isHover = enabled && CheckCollisionPointRec(mouse, rect);
    if (isHover) color = Fade(color, 0.8f);

    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, 20.0f, 1.0f);
    float buttonWidth = textSize.x + 40; // Padding for better spacing
    rect.width = buttonWidth;

    DrawRectangleRounded(rect, 0.3f, 5.0f, color);
    DrawRectangleRoundedLines(rect, 0.3f, 5.0f, BLACK);
    DrawText(text, (int)(rect.x + 10), (int)(rect.y + 10), 20, BLACK);

    static double lastClickTime = 0;
    double currentTime = GetTime();
    if (enabled && isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > 0.2) {
        lastClickTime = currentTime;
        return true;
    }
    return false;
}

void TransitionTo(Screen newScreen) {
    transitioning = true;
    transitionAlpha = 1.0f;
    nextScreen = newScreen;
}

void DrawBackButton() {
    if (currentScreen != MENU) {
        if (DrawButton({ 20, 20, 100, 40 }, "Back", GRAY, true)) TransitionTo(MENU);
    }
}

void DrawMenu() {
    ClearBackground(RAYWHITE);
    DrawText("Data Structure Visualizer", (int)(GetScreenWidth() / 2 - MeasureText("Data Structure Visualizer", 30) / 2), 80, 30, DARKBLUE);

    float centerX = GetScreenWidth() / 2 - 150; // Adjusted for smaller width
    float buttonWidth = 350; // Smaller width
    float buttonHeight = 50; // Longer height
    int textSize = 20; // Reduced text size
    float buttonSpacing = 80; // Reduced spacing between buttons

    if (DrawButton({ centerX, 200, buttonWidth, buttonHeight }, "Singly Linked List", BLUE, true)) {
        TransitionTo(LINKED_LIST);
    }
    if (DrawButton({ centerX, 200 + buttonSpacing, buttonWidth, buttonHeight }, "Hash Table", ORANGE, true)) {
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + buttonSpacing + 35, textSize, RED);
    }
    if (DrawButton({ centerX, 200 + 2 * buttonSpacing, buttonWidth, buttonHeight }, "AVL Tree", GREEN, true)) {
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + 2 * buttonSpacing + 35, textSize, RED);
    }
    if (DrawButton({ centerX, 200 + 3 * buttonSpacing, buttonWidth, buttonHeight }, "Graph", RED, true)) {
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + 3 * buttonSpacing + 35, textSize, RED);
    }
    if (DrawButton({ 20, (float)GetScreenHeight() - 120, 280, 50 }, "Fullscreen", YELLOW, true)) {
        ToggleFullscreen();
    }
}

template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

int searchResult = -1;

bool ValidateInput(const char* input, int& value) {
    try {
        value = std::stoi(input);
        return value >= 1 && value <= 9999;
    }
    catch (...) {
        return false;
    }
}


void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX) {
    ClearBackground(RAYWHITE);
    DrawBackButton();  // {20, 20, 100, 40}

    bool hasNodes = list.GetSize() > 0;

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
    }
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row2Y, buttonWidth, buttonHeight }, "Undo", ORANGE, !list.GetHistory().empty() && !list.IsAnimating())) {
        list.Undo();
    }
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight }, "Random", PINK, !list.IsAnimating())) {
        while (list.GetSize() > 0) list.Delete(list.GetNodeAt(0)->value);
        int numNodes = GetRandomValue(3, 10);
        for (int i = 0; i < numNodes; i++) {
            list.Insert(GetRandomValue(1, 9999));
        }
    }

    // History box (bigger and scrollable)
    static float historyScroll = 0.0f;  // Scroll offset
    Rectangle historyBox = { 20, 120, 200, 160 };
    DrawRectangleRec(historyBox, Fade(GRAY, 0.5f));
    DrawRectangleLinesEx(historyBox, 2, DARKGRAY);
    DrawText("History:", 25, 125, 20, BLACK);

    const auto& history = list.GetHistory();
    int maxVisible = 6;
    float scrollSpeed = 20.0f;

    // Handle scrolling within history box (inverted direction)
    if (CheckCollisionPointRec(mousePos, historyBox)) {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            historyScroll -= wheelMove * scrollSpeed;  // Inverted: up decreases scroll, down increases
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

void DrawUI(LinkedList& list, bool& isFullScreen, InputState& inputState) {
    static bool isDragging = false;
    std::string feedbackMessage = "";
    double feedbackTimer = 0.0;
    float dragStartX = 0.0f;
    // Update the animation for the linked list
    list.UpdateAnimation(GetFrameTime());  // Update the animation in each frame

    if (transitioning) {
        transitionAlpha -= 0.05f;
        if (transitionAlpha <= 0.0f) {
            currentScreen = nextScreen;
            transitioning = false;
            inputState.inputLength = 0;
            inputState.inputBuffer[0] = '\0';
            inputState.inputActive = false;
        }
    }

    if (IsKeyPressed(KEY_F11)) {
        isFullScreen = !isFullScreen;
        if (isFullScreen) {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        } else {
            SetWindowSize(800, 600);
            ToggleFullscreen();
        }
    }

    if (currentScreen == MENU) {
        DrawMenu();
    } else if (currentScreen == LINKED_LIST) {
        DrawLinkedList(list, inputState, feedbackMessage, feedbackTimer, isDragging, dragStartX);
    }

    if (transitioning) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transitionAlpha));
    }
}

