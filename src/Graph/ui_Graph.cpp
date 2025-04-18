#define _CRT_SECURE_NO_WARNINGS

#include <locale>      
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "../../header/Graph/ui_Graph.h"
#include "../../header/Graph/Graph.h"
#include "../../resource/tinyfiledialogs.h"


Graph G;
bool isOpen = true;
bool isCodeOpen = false;
Font codeFont;
Texture2D tickTexture;
Color SOFTWHITE = { 230, 230, 230, 255 };
std::string chosenAlgo = "";

bool DrawButton(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize) {
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);

    // Change button color on hover
    DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
    //DrawRectangleLinesEx(rect, 2, BLACK);

    float textX, textY;
    int textSize = FontSize;

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

        textX = rect.x + rect.width * 0.05;
        textY = rect.y + (rect.height - textSizeMeasure.y) / 2;
    }

    DrawTextEx(font, text, { textX, textY }, textSize, 1.0f, SOFTWHITE);

    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Return true if clicked
}

void DefineButtons(Rectangle& openMenuBtn, Rectangle& randomBtn, Rectangle& LoadFileBtn, Rectangle& mstBtn, Rectangle& clearBtn, Rectangle& inputBtn, Rectangle& loadMatrix, Rectangle& loadEdge) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = 150;
    float buttonHeight = 50;
    float padding = screenWidth / 100;

    float startY = HEIGHT - 65 - 250;
    openMenuBtn = { 0, startY, 50, 250 };

    randomBtn = { 50 + padding, startY, buttonWidth, buttonHeight };

    LoadFileBtn = { 50 + padding, startY + buttonHeight, buttonWidth, buttonHeight };
    loadMatrix = { 50 + 2 * padding + buttonWidth, startY + buttonHeight / 2, buttonWidth, buttonHeight };
    loadEdge = { 50 + 2 * padding + buttonWidth , startY + 3 * buttonHeight / 2, buttonWidth, buttonHeight };

    mstBtn = { 50 + padding , startY + 2 * buttonHeight, buttonWidth, buttonHeight };
    clearBtn = { 50 + padding, startY + 3 * buttonHeight, buttonWidth, buttonHeight };
    inputBtn = { 50 + padding, startY + 4 * buttonHeight, buttonWidth, buttonHeight };
}

bool isLoadFileOpen = false;
bool isTextBoxOpen = false;
Color overlayColor = { 0, 0, 0, 150 };  // Black with some transparency
ShPTextBox textBox({ 416, 266 }, { 480, 330 }, GRAY, BLACK, 500, 17);
static bool textInitialized = false;
static bool shownHelp = false;
static bool isMatrix = false;
static bool isEdgeList = true;

void DrawAndHandleButtons() {
    Rectangle randomBtn, LoadFileBtn, mstBtn, openMenuBtn, clearBtn, inputBtn, loadMatrix, loadEdge;
    DefineButtons(openMenuBtn, randomBtn, LoadFileBtn, mstBtn, clearBtn, inputBtn, loadMatrix, loadEdge);

    //Color GOLDENROD = { 218, 165, 32, 255 };

    if (isOpen) {
        if (!isTextBoxOpen) {
            if (DrawButton(openMenuBtn, "<", DARKGREEN, 1, codeFont, 0.1f * openMenuBtn.height)) {
                isOpen = false;
                isLoadFileOpen = false;
            }
            if (DrawButton(randomBtn, "Random", DARKGREEN, 2, codeFont, 17)) {
                G.RandomGraph();
            }
            if (DrawButton(LoadFileBtn, "Load File", DARKGREEN, 2, codeFont, 17)) {
                isLoadFileOpen = !isLoadFileOpen;
            }
            if (isLoadFileOpen) {
                if (DrawButton(loadMatrix, "Matrix", DARKGREEN, 2, codeFont, 17)) {
                    LoadFileGraph(1);
                }
                if (DrawButton(loadEdge, "Edge list", DARKGREEN, 2, codeFont, 17)) {
                    LoadFileGraph(2);
                }
            }
            if (DrawButton(mstBtn, "Run Kruskal", DARKGREEN, 2, codeFont, 17)) {
                isOpen = false;
                isCodeOpen = true;
                isLoadFileOpen = false;
                chosenAlgo = "Kruskal's Algorithm";
                if (!atOnce) G.StartKruskalAnimation();
                else G.RunKruskal();
            }
            if (DrawButton(clearBtn, "Clear", DARKGREEN, 2, codeFont, 17)) {
                G.clearGraph();
            }
            if (DrawButton(inputBtn, "Input Graph", DARKGREEN, 2, codeFont, 17)) {
                isTextBoxOpen = true;
                textBox.m_scrollOffset = 0;
            }
        }

        if (isTextBoxOpen) {
            if (DrawButton(openMenuBtn, "<", DARKGREEN, 1, codeFont, 0.1f * openMenuBtn.height)) {
                isOpen = false;
            }
            DrawButton(randomBtn, "Random", DARKGREEN, 2, codeFont, 17);
            DrawButton(LoadFileBtn, "Load File", DARKGREEN, 2, codeFont, 17);
            DrawButton(mstBtn, "Run Kruskal", DARKGREEN, 2, codeFont, 17);
            DrawButton(clearBtn, "Clear", DARKGREEN, 2, codeFont, 17);
            DrawButton(inputBtn, "Input Graph", DARKGREEN, 2, codeFont, 17);
        }
    }
    else {
        if (DrawButton(openMenuBtn, ">", DARKGREEN, 1, codeFont, openMenuBtn.height * 0.1f)) {
            isOpen = true;
        }
    }

    if (isTextBoxOpen) {
        isPaused = true;

        if (!textInitialized) {
            if (isEdgeList) {
                textBox.setText(G.edgeListToString());
            }
            else if (isMatrix) {
                textBox.setText(G.MatrixToString());
            }
            textInitialized = true;
        }

        Rectangle submitBtn = { 416, 600, 100, 40 };
        Rectangle clearBtn = { 526, 600, 100, 40 };

        // If the box was closed this frame, reset flag
        if (DrawTextBox()) {
            textInitialized = false;
        }

        if (isEdgeList) {
            DrawEdgeListHelpBox();
        }
        if (isMatrix) {
            DrawMatrixHelpBox();
        }

        if (DrawButton(submitBtn, "Submit", DARKGRAY, 1, codeFont, 20)) {
            // Handle submission...
            textInitialized = false;
            isTextBoxOpen = false;
            isPaused = false;
            if (isEdgeList) G.stringToEdgeList(textBox.getText());
            else if (isMatrix) G.stringToMatrix(textBox.getText());
        }
        if (DrawButton(clearBtn, "Clear", DARKGRAY, 1, codeFont, 20)) {
            textBox.clearContent();
        }

        DrawScrollBtn();
        DrawChoosingButtons();
        HandleChoosingButtons();
    }
}

void DrawChoosingButtons() {
    const char* text1 = "Edge List";
    const char* text2 = "Adjacency Matrix";
    DrawTextEx(codeFont, text1, { 940, 270 }, 25, 1, BLACK);
    DrawTextEx(codeFont, text2, { 940, 300 }, 25, 1, BLACK);
    Vector2 circle1 = { 280, 10 };
    Vector2 circle2 = { 310, 10 };
    if (isEdgeList) {
        DrawCircleLines(920, circle1.x, circle1.y, BLUE);
        DrawCircle(920, circle1.x, 8, BLUE);

        DrawCircleLines(920, circle2.x, circle2.y, BLACK);
    }
    if (isMatrix) {
        DrawCircleLines(920, circle2.x, circle2.y, BLUE);
        DrawCircle(920, circle2.x, 8, BLUE);

        DrawCircleLines(920, circle1.x, circle1.y, BLACK);
    }
}

void HandleChoosingButtons() {
    Vector2 circle1 = { 280, 10 };
    Vector2 circle2 = { 310, 10 };
    float centerX = 920;

    if (CheckCollisionPointCircle(GetMousePosition(), { centerX, circle1.x }, circle1.y) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isEdgeList = true;
        isMatrix = false;
        textInitialized = false;
        textBox.m_scrollOffset = 0;
    }
    if (CheckCollisionPointCircle(GetMousePosition(), { centerX, circle2.x }, circle2.y) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isEdgeList = false;
        isMatrix = true;
        textInitialized = false;
        textBox.m_scrollOffset = 0;
    }
}

void DrawEdgeListHelpBox() {
    // Position the help box below the choosing buttons
    float w = 295, h = 320; // Size of the help box
    float startX = 900; // Align with the first choosing button (text1X = 650)
    float startY = 325; // Just below the choosing buttons (246 + 10 for padding)
    Rectangle helpBox = { startX, startY, w, h };
    DrawRectangleRec(helpBox, WHITE);
    DrawRectangleLinesEx(helpBox, 1, BLACK);

    // Draw the help text
    float textX = helpBox.x + 10; // Padding inside the box
    float textY = helpBox.y + 10;
    float lineSpacing = 24.0f; // Space between lines
    float fontSize = 18.0f;

    DrawTextEx(codeFont, "The graph is 0-indexed", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Input format for Edge List:", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "V E", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "For each edge:", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "u v w", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Example:", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "4 6 (4 vertices, 6 edges)", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "0 1 10", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "1 2 55", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "1 3 30", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "0 3 33", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "0 2 19", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "3 2 41", { textX, textY }, fontSize, 1, BLACK);
}

void DrawMatrixHelpBox() {
    // Position the help box below the choosing buttons
    float w = 295, h = 320; // Size of the help box
    float startX = 900; // Align with the first choosing button (text1X = 650)
    float startY = 325; // Just below the choosing buttons (246 + 10 for padding)
    Rectangle helpBox = { startX, startY, w, h };
    DrawRectangleRec(helpBox, WHITE);
    DrawRectangleLinesEx(helpBox, 1, BLACK);

    // Draw the help text
    float textX = helpBox.x + 10; // Padding inside the box
    float textY = helpBox.y + 10;
    float lineSpacing = 24.0f; // Space between lines
    float fontSize = 18.0f;

    DrawTextEx(codeFont, "The graph is 0-indexed", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Input format for Adjacency", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Matrix:", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "V", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Adjacency Matrix", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "Example:", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "4 (4 vertices)", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "0  10  19  33", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "10  0  55  30", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "19  55  0  41", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
    DrawTextEx(codeFont, "33  30  41  0", { textX, textY }, fontSize, 1, BLACK);
    textY += lineSpacing;
}

void DrawScrollBtn() {
    Rectangle scrollUpBtn = { 870 , 270, 20, 20 };
    Rectangle scrollDownBtn = { 870, 572, 20, 20 };
    if (DrawButton(scrollUpBtn, "^", GRAY, 1, codeFont, 20)) {
        textBox.m_isTyping = false;
        textBox.scrollUp();
    }
    if (DrawButton(scrollDownBtn, "v", GRAY, 1, codeFont, 20)) {
        textBox.m_isTyping = false;
        textBox.scrollDown();
    }

    textBox.DrawScrollBar();
}

void DefineTextBoxElements(Rectangle& menu) {
    float w = 800, h = 400;
    float startX = (WIDTH - w) / 2, startY = (HEIGHT - h) / 2;
    float padding = w / 50;

    menu = { startX, startY, w, h };
}

bool DrawTextBox() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), overlayColor);
    Rectangle menu;
    DefineTextBoxElements(menu);
    DrawRectangleRec(menu, WHITE);
    textBox.render();
    textBox.update();

    Rectangle closeBtn = { menu.x + menu.width - 100, menu.y, 100, 40 };

    // Check for close
    bool closed = false;
    if (DrawButton(closeBtn, "Close", RED, 1, codeFont, 20) || IsKeyPressed(KEY_ESCAPE)) {
        isTextBoxOpen = false;
        isPaused = false;
        closed = true;
    }

    DrawRectangleLinesEx(menu, 1, BLACK);

    return closed;
}

void DrawOutLine() {
    const char* title = "MINIMUM SPANNING TREE";
    float textSize = 30; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(codeFont, title, textSize, 1);

    // Center the text within the button
    float textX = (WIDTH - textSizeMeasure.x) / 2;
    float textY = (50 - textSizeMeasure.y) / 2;

    DrawRectangle(0, 0, WIDTH, 50, BLACK);
    DrawTextEx(codeFont, title, { textX, textY }, textSize, 1, SOFTWHITE);
    DrawRectangle(0, HEIGHT - 50, WIDTH, 50, BLACK);
    DrawRectangle(0, 50, 50, HEIGHT - 100, BLACK);
    DrawRectangle(WIDTH - 50, 50, 50, HEIGHT - 100, BLACK);
}

bool LoadFileGraph(int choice) {
    const char* path = tinyfd_openFileDialog("Choose A File", "", 0, nullptr, nullptr, 0);

    if (path == nullptr) return false;

    // Convert UTF-8 path to wide string
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wPath = converter.from_bytes(path);

    std::wifstream fin(wPath);  // Use wide file stream
    fin.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));  // Handle UTF-8

    if (!fin) {
        message = "Error: Could not open file!";
        return false;
    }

    G.loadFromFile(fin, choice);

    fin.close();

    return true;
}

void DefineCodeTableElements(Rectangle& openBtn, Rectangle& startAlgo1, Rectangle& startAlgo2, Rectangle& forLoop, Rectangle& condition, Rectangle& add, Rectangle& skip, Rectangle& done) {
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
        if (DrawButton(openBtn, ">", BLUE, 1, codeFont, openBtn.height * 0.1f)) {
            isCodeOpen = false;
        }

        const char* algo1 = "Sort edges by increasing weight";
        DrawButton(startAlgo1, algo1, (stateOfCode == 1) ? BLACK : BLUE, 2, codeFont, 17);

        const char* algo2 = "T={ }";
        DrawButton(startAlgo2, algo2, (stateOfCode == 1) ? BLACK : BLUE, 2, codeFont, 17);

        const char* forloop = "for ( i = 0; i < edgeList.size; i++)";
        DrawButton(forLoop, forloop, (stateOfCode == 2) ? BLACK : BLUE, 2, codeFont, 17);

        const char* conditionText = "   if adding e = edgeList[i] does not form a cycle";
        DrawButton(condition, conditionText, (stateOfCode == 3) ? BLACK : BLUE, 2, codeFont, 17);

        const char* addText = "       add e to T";
        DrawButton(add, addText, (stateOfCode == 4) ? BLACK : BLUE, 2, codeFont, 17);

        const char* skipText = "   else ignore e";
        DrawButton(skip, skipText, (stateOfCode == 5) ? BLACK : BLUE, 2, codeFont, 17);

        const char* doneText = "MST = T";
        DrawButton(done, doneText, (stateOfCode == 6) ? BLACK : BLUE, 2, codeFont, 17);
    }
    else {
        if (DrawButton(openBtn, "<", BLUE, 1, codeFont, openBtn.height * 0.1f)) {
            isCodeOpen = true;
        }
    }
}

void DrawChosenAlgo() {
    float h = 50.0f, w = 330.0f;
    float startY = 50.0f;
    float startX = WIDTH - 50 - w;

    Rectangle rect = { startX, startY, w, h };
    float textSize = 25; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(codeFont, chosenAlgo.c_str(), textSize, 1);

    // Center the text within the button
    float textX = rect.x + (rect.width - textSizeMeasure.x) / 2;
    float textY = rect.y + (rect.height - textSizeMeasure.y) / 2;

    DrawTextEx(codeFont, chosenAlgo.c_str(), { textX, textY }, textSize, 1.0f, BLACK);
}

void DrawCurrentStep() {
    float h = 50.0f, w = 2 * WIDTH / 3.0;
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
    y = HEIGHT - 25;
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

    DrawRectangle(minX, y - 3, maxX - minX, 6, GRAY);

    DrawRectangleRounded({ sliderX - 10, y - 10, 20, 20 }, 0.2f, 20, SOFTWHITE);

    float speedValue = (180.0f - animationSpeed) / 90.0f + 0.5f;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << speedValue;
    std::string speedText = oss.str() + "x";

    Vector2 textPos = MeasureTextEx(codeFont, speedText.c_str(), 20, 1);

    DrawTextEx(codeFont, speedText.c_str(), { maxX + 10, y - textPos.y / 2.0f }, 20, 1, SOFTWHITE);
}

void DefinePauseButton(Rectangle& pauseBtn) {
    float w = 100.0f, h = 30.0f;
    float startX = (WIDTH - w) / 2, startY = HEIGHT - 25 - h / 2;

    pauseBtn = { startX, startY, w, h };
}

void DrawPauseButton() {
    Rectangle pauseBtn;
    DefinePauseButton(pauseBtn);

    if (isPaused) {
        if (DrawButton(pauseBtn, "Resume", GREEN, 1, codeFont, 30) || IsKeyPressed(KEY_SPACE)) {
            isPaused = false;
        }
    }
    else {
        if (DrawButton(pauseBtn, "Pause", RED, 1, codeFont, 30) || IsKeyPressed(KEY_SPACE)) {
            isPaused = true;
        }
    }
}

void DrawGraphProgram() {
    //BeginDrawing();
    

    DrawOutLine();

    G.DrawGraph();

    DrawCodeTable();
    DrawAndHandleButtons();
    DrawChosenAlgo();
    DrawCurrentStep();
    DrawSpeedSlider();
    UpdateSlider();
    DrawPauseButton();
    DrawRunAtOnceBtn();

    //EndDrawing();
}

bool atOnce = false;
void DrawRunAtOnceBtn() {
    const char* text = "Run At Once";

    DrawTextEx(codeFont, text, { 1100, 863}, 25, 1, RAYWHITE);

    Rectangle btn = { 1255, 863, 25, 25 };

    if (CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        atOnce = !atOnce;
    }

    Color color = LIGHTGRAY;
    if (CheckCollisionPointRec(GetMousePosition(), btn)) {
        color = Fade(color, 0.7f);
    }

    DrawRectangleRec(btn, color);

    if (atOnce) {
        float scale = 25.0f / tickTexture.width;

        float scaledWidth = tickTexture.width * scale;
        float scaledHeight = tickTexture.height * scale;

        float x = btn.x + (btn.width - scaledWidth) / 2;
        float y = btn.y + (btn.height - scaledHeight) / 2;

        Vector2 pos = { x, y };
        DrawTextureEx(tickTexture, pos, 0.0f, scale, WHITE);
    }
}