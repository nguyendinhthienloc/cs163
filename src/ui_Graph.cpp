#define _CRT_SECURE_NO_WARNINGS

#include "ui_Graph.h"
#include "Graph.h"
#include "tinyfiledialogs.h"
#include <locale>      // For codecvt
#include <codecvt>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

Graph G;
bool isOpen = true;
bool isCodeOpen = false;
Font codeFont;
Color SOFTWHITE = { 230, 230, 230, 255 };
std::string chosenAlgo = "";

bool DrawButton(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize) {
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);

    // Change button color on hover
    DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
    //DrawRectangleLinesEx(rect, 2, BLACK);

    float textX, textY;
    int textSize=FontSize;

    if (shift == 1) {
        // Dynamically adjust text size based on button height
       //textSize = rect.height * 0.1; // 50% of button height
        Vector2 textSizeMeasure = MeasureTextEx(font, text, textSize, 1);

        // Center the text within the button
        textX = rect.x + (rect.width - textSizeMeasure.x) / 2;
        textY = rect.y + (rect.height - textSizeMeasure.y) / 2;
    }
    else {
        //textSize = 17;// rect.height * 0.3;
        Vector2 textSizeMeasure = MeasureTextEx(font, text, textSize, 1);

        textX = rect.x+rect.width * 0.05;
        textY = rect.y + (rect.height - textSizeMeasure.y) / 2;
    }

    DrawTextEx(font, text, { textX, textY }, textSize, 1.0f, SOFTWHITE);

    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Return true if clicked
}

void DefineButtons(Rectangle& openMenuBtn, Rectangle& randomBtn, Rectangle& LoadFileBtn, Rectangle& mstBtn, Rectangle& clearBtn) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = 150;
    float buttonHeight = 50;
    float padding = screenWidth / 100;

    float startY = HEIGHT - 65 - 200;
    openMenuBtn = {0, startY, 50, 200};

    randomBtn = { 50+padding, startY, buttonWidth, buttonHeight };
    LoadFileBtn = { 50+padding, startY+buttonHeight, buttonWidth, buttonHeight };
    mstBtn = { 50+ padding , startY+2*buttonHeight, buttonWidth, buttonHeight };
    clearBtn = { 50 + padding, startY + 3 * buttonHeight, buttonWidth, buttonHeight };
}

void DrawAndHandleButtons() {
    Rectangle randomBtn, LoadFileBtn, mstBtn, openMenuBtn, clearBtn;
    DefineButtons(openMenuBtn, randomBtn, LoadFileBtn, mstBtn, clearBtn);

    //Color GOLDENROD = { 218, 165, 32, 255 };

    if (isOpen) {
        if (DrawButton(openMenuBtn, "<", DARKGREEN, 1, codeFont, 0.1f*openMenuBtn.height)) {
            isOpen = false;
        }
        if (DrawButton(randomBtn, "Random", DARKGREEN, 2, codeFont, 17)) {
            G.RandomGraph();
        }
        if (DrawButton(LoadFileBtn, "Load File", DARKGREEN, 2, codeFont, 17)) {
            LoadFileGraph();
        }
        if (DrawButton(mstBtn, "Run Kruskal", DARKGREEN, 2, codeFont, 17)) {
            isOpen = false;
            isCodeOpen = true;
            chosenAlgo = "Kruskal's Algorithm";
            G.StartKruskalAnimation();
        }
        if (DrawButton(clearBtn, "Clear", DARKGREEN, 2, codeFont, 17)) {
            G.clearGraph();
        }
    }
    else {
        if (DrawButton(openMenuBtn, ">", DARKGREEN, 1, codeFont, openMenuBtn.height*0.1f)) {
            isOpen = true;
        }
    }
}

void DrawOutLine() {
    const char* title = "MINIMUM SPANNING TREE";
    float textSize = 30; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(codeFont, title, textSize, 1);

    // Center the text within the button
    float textX =  (WIDTH - textSizeMeasure.x) / 2;
    float textY = (50 - textSizeMeasure.y) / 2;

    DrawRectangle(0, 0, WIDTH, 50, BLACK);
    DrawTextEx(codeFont, title, { textX, textY }, textSize, 1, SOFTWHITE);
    DrawRectangle(0, HEIGHT - 50, WIDTH, 50, BLACK);
    DrawRectangle(0, 50, 50, HEIGHT - 100, BLACK);
    DrawRectangle(WIDTH - 50, 50, 50, HEIGHT - 100, BLACK);
}

bool LoadFileGraph() {
    const char* path = tinyfd_openFileDialog("Choose A File", "", 0, nullptr, nullptr, 0);

    if (path == nullptr) return false;

    // Convert UTF-8 path to wide string
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wPath = converter.from_bytes(path);

    std::wifstream fin(wPath);  // Use wide file stream
    fin.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));  // Handle UTF-8

    if (!fin) {
        std::cerr << "Error: Could not open file!\n";
        return false;
    }

    G.loadFromFile(fin);

    fin.close();

    return true;
}

void DefineCodeTableElements(Rectangle& openBtn, Rectangle& startAlgo1, Rectangle& startAlgo2, Rectangle& forLoop, Rectangle& condition, Rectangle& add, Rectangle&  skip, Rectangle& done) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = 510;
    float buttonHeight = 40;
    float padding = screenWidth / 100;

    float startY = screenHeight - 65 - 280;
    float startX = screenWidth - 50;
    openBtn = { startX, startY, 50, 280 };

    startAlgo1 = { startX - padding - buttonWidth, startY, buttonWidth, buttonHeight };
    startAlgo2 = { startX - padding - buttonWidth , startY + buttonHeight, buttonWidth, buttonHeight };
    forLoop = { startX - padding - buttonWidth, startY + 2 * buttonHeight, buttonWidth, buttonHeight };
    condition = { startX - padding - buttonWidth, startY + 3 * buttonHeight, buttonWidth, buttonHeight };
    add = { startX - padding - buttonWidth, startY + 4 * buttonHeight, buttonWidth, buttonHeight };
    skip = { startX - padding - buttonWidth, startY + 5 * buttonHeight, buttonWidth, buttonHeight };
    done = { startX - padding - buttonWidth, startY + 6 * buttonHeight, buttonWidth, buttonHeight };
}

void DrawCodeTable() {
    Rectangle openBtn, startAlgo1, startAlgo2, forLoop, condition, add, skip, done;
    DefineCodeTableElements(openBtn, startAlgo1, startAlgo2, forLoop, condition, add, skip, done);
    if (isCodeOpen) {
        if (DrawButton(openBtn, ">", BLUE, 1, codeFont, openBtn.height*0.1f)) {
            isCodeOpen = false;
        }

        const char* algo1 = "Sort edges by increasing weight";
        DrawButton(startAlgo1, algo1, (stateOfCode==1)? BLACK : BLUE, 2, codeFont, 17);

        const char* algo2 = "T={ }";
        DrawButton(startAlgo2, algo2, (stateOfCode == 1) ? BLACK : BLUE, 2, codeFont, 17);

        const char* forloop = "for ( i = 0; i < edgeList.size; i++)";
        DrawButton(forLoop, forloop, (stateOfCode == 2) ? BLACK : BLUE, 2, codeFont, 17);

        const char* conditionText = "   if adding e = edgeList[i] does not form a cycle";
        DrawButton(condition, conditionText, (stateOfCode == 3) ? BLACK : BLUE, 2, codeFont, 17);

        const char* addText = "       add e to T";
        DrawButton(add, addText, (stateOfCode == 4) ? BLACK: BLUE, 2, codeFont, 17);

        const char* skipText = "   else ignore e";
        DrawButton(skip, skipText, (stateOfCode == 5) ? BLACK : BLUE, 2, codeFont, 17);

        const char* doneText = "MST = T";
        DrawButton(done, doneText, (stateOfCode == 6) ? BLACK : BLUE, 2, codeFont, 17);
    }
    else {
        if (DrawButton(openBtn, "<", BLUE, 1, codeFont, openBtn.height*0.1f)) {
            isCodeOpen = true;
        }
    }
}

void DrawChosenAlgo() {
    float h = 50.0f, w = 330.0f;
    float startY = 50.0f+h;
    float startX = WIDTH - 50-w;

    Rectangle rect = { startX, startY, w, h };
    float textSize = 25; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(codeFont, chosenAlgo.c_str(), textSize, 1);

    // Center the text within the button
    float textX = rect.x + (rect.width - textSizeMeasure.x) / 2;
    float textY = rect.y + (rect.height - textSizeMeasure.y) / 2;

    DrawTextEx(codeFont, chosenAlgo.c_str(), {textX, textY}, textSize, 1.0f, BLACK);
}

void DrawCurrentStep() {
    float h = 50.0f, w = 2*WIDTH/3.0;
    float startX = 0, startY = HEIGHT - 50 - h;

    Rectangle rect = { startX, startY, w, h };

    float textSize = 40; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(codeFont, message.c_str(), textSize, 1);

    // Center the text within the button
    float textX = rect.x + (rect.width - textSizeMeasure.x) / 2;
    float textY = rect.y + (rect.height - textSizeMeasure.y) / 2;

    DrawTextEx(codeFont, message.c_str(), { textX, textY }, textSize, 1.0f, BLACK);
}

float sliderX = 166;
bool isDragging = false;

void DefineSpeedSlider(float& minX, float& maxX, float& y, float& minValue, float& maxValue) {
    minX = 100;
    maxX = 300;
    y = HEIGHT-25;
    minValue = 45.0f;
    maxValue = 180.0f;
}

void UpdateSlider() {
    float minX, maxX, y, minValue, maxValue;
    DefineSpeedSlider(minX, maxX, y, minValue, maxValue);

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        mousePos.x >= sliderX - 10 && mousePos.x <= sliderX + 10 &&
        mousePos.y >= y - 10 && mousePos.y <= y + 10) {
        isDragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) isDragging = false;

    if (isDragging) {
        sliderX = mousePos.x;
        if (sliderX < minX) sliderX = minX;
        if (sliderX > maxX) sliderX = maxX;
    }

    animationSpeed = maxValue - ((sliderX - minX) / (maxX - minX)) * (maxValue - minValue);
}

void DrawSpeedSlider() {
    float minX, maxX, y, minValue, maxValue;
    DefineSpeedSlider(minX, maxX, y, minValue, maxValue);

    DrawRectangle(minX, y - 3, maxX-minX, 6, GRAY);

    DrawRectangleRounded({ sliderX-10, y - 10, 20, 20}, 0.2f, 20, SOFTWHITE);

    float speedValue = (180.0f-animationSpeed)/90.0f+0.5f;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << speedValue;
    std::string speedText = oss.str()+"x";

    Vector2 textPos = MeasureTextEx(codeFont, speedText.c_str(), 20, 1);

    DrawTextEx(codeFont, speedText.c_str(), { maxX + 10, y-textPos.y/2.0f }, 20, 1, SOFTWHITE);
}