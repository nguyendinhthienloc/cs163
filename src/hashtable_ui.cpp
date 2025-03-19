#include "raylib.h"
#include "hashtable_ui.h"
#include "HashTable.h"
#include "tinyfiledialogs.h"
#include <string>
#include <fstream>
#include <locale>   
#include <codecvt>

Texture2D undoIcon;
Texture2D redoIcon;

void LoadIcons() {
    undoIcon = LoadTexture("icons/undo.png");
    redoIcon = LoadTexture("icons/redo.png");
}

HashTable ht;

// Define screen variable (default: MENU)
Screen currentScreen = MENU;

// Button rendering function with animations
bool DrawButton(Rectangle rect, const char* text, Color color) {
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);

    // Change button color on hover
    DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
    DrawRectangleLinesEx(rect, 2, BLACK);

    // Dynamically adjust text size based on button height
    int textSize = rect.height * 0.5; // 50% of button height
    Vector2 textSizeMeasure = MeasureTextEx(GetFontDefault(), text, textSize, 1);

    // Center the text within the button
    float textX = rect.x + (rect.width - textSizeMeasure.x) / 2;
    float textY = rect.y + (rect.height - textSizeMeasure.y) / 2;

    DrawText(text, textX, textY, textSize, BLACK);

    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Return true if clicked
}

// Global text input buffer
static char textInput[50];
static int letterCount = 0;

// Function to draw the input box
bool isInputActive = false;  // Track if input box is active

// Draw Main Menu UI
void DrawMenu() {
    ClearBackground(RAYWHITE);
    DrawText("Data Structure Visualizer", 180, 80, 30, DARKBLUE);
    
    if (DrawButton({250, 200, 300, 50}, "Singly Linked List", SKYBLUE)) {
        currentScreen = LINKED_LIST;
    }
    if (DrawButton({250, 270, 300, 50}, "Hash Table (Chaining)", ORANGE)) {
        currentScreen = HASH_TABLE;
    }
    if (DrawButton({250, 340, 300, 50}, "2-3 Tree", GREEN)) {
        currentScreen = TWO_THREE_TREE;
    }
    if (DrawButton({250, 410, 300, 50}, "Shortest Path (Graph)", RED)) {
        currentScreen = GRAPH;
    }
}

void DrawInputBox(Rectangle inputBox) {
    Vector2 mouse = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mouse, inputBox);

    // Check if the input box is clicked
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isInputActive = isHovered;
    }

    // Change color when active
    Color boxColor = WHITE;
    Color borderColor = isInputActive ? RED : BLACK;  // Change border color when active

    DrawRectangleRec(inputBox, boxColor);
    DrawRectangleLinesEx(inputBox, 2, borderColor);
    DrawText(textInput, inputBox.x + 5, inputBox.y + inputBox.height / 4, 20, BLACK);
}

// Function to handle text input
void HandleTextInput() {
    int key = GetCharPressed();
    while (key > 0) {
        if (letterCount < 49 && key >= '0' && key <= '9') { // Valid characters
            textInput[letterCount] = (char)key;
            letterCount++;
            textInput[letterCount] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
        letterCount--;
        textInput[letterCount] = '\0';
    }
}

void ProcessButtonClick(void (*operation)(int)) {
    if (letterCount > 0) {
        int value = atoi(textInput); // Convert text input to integer
        operation(value); // Call the respective function (Insert, Delete, or Search)
        textInput[0] = '\0'; // Clear input box
        letterCount = 0;
    }
}

// **Draw UI and handle button actions**
void DefineUIElements(Rectangle& insertBtn, Rectangle& deleteBtn, Rectangle& searchBtn, Rectangle& inputBox,
    Rectangle& randomBtn, Rectangle& clearBtn, Rectangle& undoBtn, Rectangle& redoBtn, Rectangle& loadFileBtn) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = screenWidth / 8;
    float buttonHeight = screenHeight / 12;
    float padding = screenWidth / 50;

    float totalWidth = 3 * (buttonWidth + padding) + (screenWidth / 3);
    float startX = screenWidth - totalWidth - padding;
    float startY = padding;

    insertBtn = { startX, startY, buttonWidth, buttonHeight };
    deleteBtn = { startX + buttonWidth + padding, startY, buttonWidth, buttonHeight };
    searchBtn = { startX + 2 * (buttonWidth + padding), startY, buttonWidth, buttonHeight };
    inputBox = { startX + 3 * (buttonWidth + padding), startY, screenWidth / 3, buttonHeight };

    randomBtn = { padding, screenHeight - buttonHeight - padding, buttonWidth, buttonHeight };
    clearBtn = { randomBtn.x + buttonWidth + padding, randomBtn.y, buttonWidth, buttonHeight };

    float loadFileWidth = 2 * buttonWidth + padding;
    loadFileBtn = { padding, randomBtn.y - buttonHeight - padding, loadFileWidth, buttonHeight };

    redoBtn = { screenWidth - padding - buttonWidth, screenHeight - buttonHeight - padding, buttonWidth, buttonHeight };
    undoBtn = { redoBtn.x - buttonWidth - padding, redoBtn.y, buttonWidth, buttonHeight };
}

void DrawAndHandleButtons(const Rectangle& insertBtn, const Rectangle& deleteBtn, const Rectangle& searchBtn,
    const Rectangle& randomBtn, const Rectangle& clearBtn, const Rectangle& loadFileBtn) {
    if (DrawButton(insertBtn, "Insert", GREEN)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            ht.StartSearch(v, true); // Shows calculation
            ht.SetPendingOperation(HashTable::PendingOperation::INSERT, v);
            });
    }

    if (DrawButton(deleteBtn, "Delete", RED)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            ht.StartSearch(v, false); // Shows calculation
            ht.SetPendingOperation(HashTable::PendingOperation::DELETE, v);
            });
    }

    if (DrawButton(searchBtn, "Search", BLUE)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            ht.StartSearch(v, false); // Shows calculation
            });
    }

    if (DrawButton(randomBtn, "Random", ORANGE)) {
        ht.ResetColors();
        ht.searchMessage = "Generated random values.";
        ht.RandomInsert(30, 0, 100);
    }

    if (DrawButton(clearBtn, "Clear", RED)) {
        ht.ResetColors();
        ht.searchMessage = "Table cleared.";
        ht.Clear();
    }

    if (DrawButton(loadFileBtn, "Load File", PURPLE)) { // New button handling
        ht.ResetColors();
        LoadFromFile(); // Specify your file name here
        ht.searchMessage = "Loaded values from file.";
    }
}

void LoadFromFile() {
    const char* filters[] = { "*.txt" };
    const char* filePath = tinyfd_openFileDialog(
        "Select a Text File", "", 1, filters, "Text Files", 0
    );

    if (!filePath) {
        ht.searchMessage = "File selection canceled.";
        return;
    }

    // Convert UTF-8 path to UTF-16 for Windows
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideFilePath = converter.from_bytes(filePath);

    // Use _wfopen_s to open the file
    FILE* file = nullptr;
    errno_t err = _wfopen_s(&file, wideFilePath.c_str(), L"r");
    if (err != 0 || file == nullptr) {
        ht.searchMessage = TextFormat("Failed to open file: %s", filePath);
        return;
    }

    ht.Clear();
    ht.ResetTableOffset();

    int value;
    int count = 0;
    while (fscanf_s(file, "%d", &value) == 1) {
        ht.Insert(value, true);
        count++;
    }

    fclose(file);
    ht.searchMessage = TextFormat("Loaded %d values from %s", count, filePath);
}

void DrawUndoRedoButtons(const Rectangle& undoBtn, const Rectangle& redoBtn) {
    if (CheckCollisionPointRec(GetMousePosition(), undoBtn)) DrawRectangleRec(undoBtn, GRAY);
    DrawTexture(undoIcon, undoBtn.x + (undoBtn.width - undoIcon.width) / 2, undoBtn.y + (undoBtn.height - undoIcon.height) / 2, BLACK);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), undoBtn)) {
        ht.ResetColors();
        ht.Undo();
    }

    if (CheckCollisionPointRec(GetMousePosition(), redoBtn)) DrawRectangleRec(redoBtn, GRAY);
    DrawTexture(redoIcon, redoBtn.x + (redoBtn.width - redoIcon.width) / 2, redoBtn.y + (redoBtn.height - redoIcon.height) / 2, BLACK);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), redoBtn)) {
        ht.ResetColors();
        ht.Redo();
    }
}

// Draw Back Button on visualization screens
void DrawBackButton() {
    if (DrawButton({20, 20, 100, 40}, "Back", GRAY)) {
        if (currentScreen == HASH_TABLE) {
            ht.cleanUp();
        }
        currentScreen = MENU;
    }
}

void DrawHashTable() {
    Rectangle insertBtn, deleteBtn, searchBtn, inputBox, randomBtn, clearBtn, undoBtn, redoBtn, loadFileBtn;
    DefineUIElements(insertBtn, deleteBtn, searchBtn, inputBox, randomBtn, clearBtn, undoBtn, redoBtn, loadFileBtn);

    DrawAndHandleButtons(insertBtn, deleteBtn, searchBtn, randomBtn, clearBtn, loadFileBtn);
    DrawUndoRedoButtons(undoBtn, redoBtn);

    ht.HandleTableDragging();
    ht.UpdateDeletionAnimation();
    ht.UpdateSearchAnimation();
    ht.Draw();

    DrawInputBox(inputBox);
    HandleTextInput();
}

// Draw UI based on the current screen
void DrawUI() {
    if (currentScreen == MENU) {
        DrawMenu();
    } else {
        DrawBackButton(); // Adds a back button for navigation
        if (currentScreen == LINKED_LIST) {
            //DrawLinkedList();
        } else if (currentScreen == HASH_TABLE) {
            DrawHashTable();
        } else if (currentScreen == TWO_THREE_TREE) {
            //DrawTwoThreeTree();
        } else if (currentScreen == GRAPH) {
            //DrawGraph();
        }
    }
}