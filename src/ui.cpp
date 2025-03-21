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

template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

int searchResult = -1;

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

    float centerX = GetScreenWidth() / 2 - 125; // Adjusted width for narrower buttons
    float buttonWidth = 200; // Reduced button width
    float buttonHeight = 50;
    int textSize = 20;
    float buttonSpacing = 80;

    Vector2 mouse = GetMousePosition();
    static double lastClickTime = 0;
    double currentTime = GetTime();
    const float CLICK_DELAY = 0.1f;  // Reduced from 0.2s for faster response

    // Button hover/click animation states
    static float hoverScale[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };  // Scale for each button
    const float HOVER_SCALE = 1.05f;  // Slight scale increase on hover
    const float HOVER_SPEED = 10.0f;  // Speed of scale transition

    // Button 1: Singly Linked List
    Rectangle btn1Rect = { centerX, 200, buttonWidth, buttonHeight };
    bool btn1Hover = CheckCollisionPointRec(mouse, btn1Rect);
    hoverScale[0] += (btn1Hover ? (HOVER_SCALE - hoverScale[0]) : (1.0f - hoverScale[0])) * HOVER_SPEED * GetFrameTime();
    Color btn1Color = BLUE;
    if (btn1Hover) btn1Color = Fade(btn1Color, 0.9f);  // Brighter on hover
    DrawRectangleRounded({ btn1Rect.x, btn1Rect.y, btn1Rect.width * hoverScale[0], btn1Rect.height * hoverScale[0] }, 0.3f, 5.0f, btn1Color);
    DrawRectangleRoundedLines(btn1Rect, 0.3f, 5.0f, BLACK);
    DrawText("Singly Linked List", (int)(btn1Rect.x + 10), (int)(btn1Rect.y + 10), textSize, BLACK);
    if (btn1Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        TransitionTo(LINKED_LIST);
        hoverScale[0] = 0.95f;  // Brief scale down on click
    }

    // Button 2: Hash Table
    Rectangle btn2Rect = { centerX, 200 + buttonSpacing, buttonWidth, buttonHeight };
    bool btn2Hover = CheckCollisionPointRec(mouse, btn2Rect);
    hoverScale[1] += (btn2Hover ? (HOVER_SCALE - hoverScale[1]) : (1.0f - hoverScale[1])) * HOVER_SPEED * GetFrameTime();
    Color btn2Color = ORANGE;
    if (btn2Hover) btn2Color = Fade(btn2Color, 0.9f);
    DrawRectangleRounded({ btn2Rect.x, btn2Rect.y, btn2Rect.width * hoverScale[1], btn2Rect.height * hoverScale[1] }, 0.3f, 5.0f, btn2Color);
    DrawRectangleRoundedLines(btn2Rect, 0.3f, 5.0f, BLACK);
    DrawText("Hash Table", (int)(btn2Rect.x + 10), (int)(btn2Rect.y + 10), textSize, BLACK);
    if (btn2Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + buttonSpacing + 35, textSize, RED);
        hoverScale[1] = 0.95f;
    } 

    // Button 3: AVL Tree
    Rectangle btn3Rect = { centerX, 200 + 2 * buttonSpacing, buttonWidth, buttonHeight };
    bool btn3Hover = CheckCollisionPointRec(mouse, btn3Rect);
    hoverScale[2] += (btn3Hover ? (HOVER_SCALE - hoverScale[2]) : (1.0f - hoverScale[2])) * HOVER_SPEED * GetFrameTime();
    Color btn3Color = GREEN;
    if (btn3Hover) btn3Color = Fade(btn3Color, 0.9f);
    DrawRectangleRounded({ btn3Rect.x, btn3Rect.y, btn3Rect.width * hoverScale[2], btn3Rect.height * hoverScale[2] }, 0.3f, 5.0f, btn3Color);
    DrawRectangleRoundedLines(btn3Rect, 0.3f, 5.0f, BLACK);
    DrawText("AVL Tree", (int)(btn3Rect.x + 10), (int)(btn3Rect.y + 10), textSize, BLACK);
    if (btn3Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + 2 * buttonSpacing + 35, textSize, RED);
        hoverScale[2] = 0.95f;
    } 
    // Button 4: Graph
    Rectangle btn4Rect = { centerX, 200 + 3 * buttonSpacing, buttonWidth, buttonHeight };
    bool btn4Hover = CheckCollisionPointRec(mouse, btn4Rect);
    hoverScale[3] += (btn4Hover ? (HOVER_SCALE - hoverScale[3]) : (1.0f - hoverScale[3])) * HOVER_SPEED * GetFrameTime();
    Color btn4Color = VIOLET;
    if (btn4Hover) btn4Color = Fade(btn4Color, 0.9f);
    DrawRectangleRounded({ btn4Rect.x, btn4Rect.y, btn4Rect.width * hoverScale[3], btn4Rect.height * hoverScale[3] }, 0.3f, 5.0f, btn4Color);
    DrawRectangleRoundedLines(btn4Rect, 0.3f, 5.0f, BLACK);
    DrawText("Graph", (int)(btn4Rect.x + 10), (int)(btn4Rect.y + 10), textSize, BLACK);
    if (btn4Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        DrawText("COMING SOON", (int)(centerX + buttonWidth / 2 - 60), 200 + 3 * buttonSpacing + 35, textSize, RED);
        hoverScale[3] = 0.95f;
    } 

    // Button 5: Fullscreen
    Rectangle btn5Rect = { 20, (float)GetScreenHeight() - 120, 200, 50 }; // Adjusted width for consistency
    bool btn5Hover = CheckCollisionPointRec(mouse, btn5Rect);
    hoverScale[4] += (btn5Hover ? (HOVER_SCALE - hoverScale[4]) : (1.0f - hoverScale[4])) * HOVER_SPEED * GetFrameTime();
    Color btn5Color = YELLOW;
    if (btn5Hover) btn5Color = Fade(btn5Color, 0.9f);
    DrawRectangleRounded({ btn5Rect.x, btn5Rect.y, btn5Rect.width * hoverScale[4], btn5Rect.height * hoverScale[4] }, 0.3f, 5.0f, btn5Color);
    DrawRectangleRoundedLines(btn5Rect, 0.3f, 5.0f, BLACK);
    DrawText("Fullscreen", (int)(btn5Rect.x + 10), (int)(btn5Rect.y + 10), textSize, BLACK);
    if (btn5Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        ToggleFullscreen();
        hoverScale[4] = 0.95f;
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

void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX) {
    ClearBackground(RAYWHITE);
    DrawBackButton();

    bool hasNodes = list.GetSize() > 0;
    static bool undoInProgress = false;  // Flag to prevent multiple undos

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
        undoInProgress = false;  // Reset on clear
    }
    if (DrawButton({ startX + buttonWidth + buttonSpacing, row2Y, buttonWidth, buttonHeight }, "Undo", ORANGE, !list.GetHistory().empty() && !list.IsAnimating() && !undoInProgress)) {
        list.Undo();
        undoInProgress = true;  // Set flag to prevent further undos
    }
    if (DrawButton({ startX + 2 * (buttonWidth + buttonSpacing), row2Y, buttonWidth, buttonHeight }, "Random", PINK, !list.IsAnimating())) {
        while (list.GetSize() > 0) list.Delete(list.GetNodeAt(0)->value);
        int numNodes = GetRandomValue(3, 10);
        for (int i = 0; i < numNodes; i++) {
            list.Insert(GetRandomValue(1, 9999));
        }
        undoInProgress = false;  // Reset on random
    }

    // Reset undoInProgress when animation finishes
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

