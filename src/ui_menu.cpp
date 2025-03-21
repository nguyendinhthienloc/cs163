#include "ui_menu.h"
#include <cstring>

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
    float buttonWidth = textSize.x + 40;
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

    float centerX = GetScreenWidth() / 2 - 125;
    float buttonWidth = 200;
    float buttonHeight = 50;
    int textSize = 20;
    float buttonSpacing = 80;

    Vector2 mouse = GetMousePosition();
    static double lastClickTime = 0;
    double currentTime = GetTime();
    const float CLICK_DELAY = 0.1f;

    static float hoverScale[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    const float HOVER_SCALE = 1.05f;
    const float HOVER_SPEED = 10.0f;

    // Button 1: Singly Linked List
    Rectangle btn1Rect = { centerX, 200, buttonWidth, buttonHeight };
    bool btn1Hover = CheckCollisionPointRec(mouse, btn1Rect);
    hoverScale[0] += (btn1Hover ? (HOVER_SCALE - hoverScale[0]) : (1.0f - hoverScale[0])) * HOVER_SPEED * GetFrameTime();
    Color btn1Color = BLUE;
    if (btn1Hover) btn1Color = Fade(btn1Color, 0.9f);
    DrawRectangleRounded({ btn1Rect.x, btn1Rect.y, btn1Rect.width * hoverScale[0], btn1Rect.height * hoverScale[0] }, 0.3f, 5.0f, btn1Color);
    DrawRectangleRoundedLines(btn1Rect, 0.3f, 5.0f, BLACK);
    DrawText("Singly Linked List", (int)(btn1Rect.x + 10), (int)(btn1Rect.y + 10), textSize, BLACK);
    if (btn1Hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastClickTime) > CLICK_DELAY) {
        lastClickTime = currentTime;
        TransitionTo(LINKED_LIST);
        hoverScale[0] = 0.95f;
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
    Rectangle btn5Rect = { 20, (float)GetScreenHeight() - 120, 200, 50 };
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