#include "../../header/Hash Table/hashtable_ui.h"


HashTable ht;

// Button rendering function with animations
bool DrawButton(Rectangle rect, const char* text, Color color) {
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);

    // Draw button background and border
    DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
    DrawRectangleLinesEx(rect, 2, BLACK);

    // Dynamically adjust text size based on button height
    float textSize = rect.height * 0.5f; // 50% of button height
    Font font = codeFont; // Use default font for consistency
    float spacing = 1.0f; // Font spacing for measurement

    // Measure text size
    Vector2 textSizeMeasure = MeasureTextEx(font, text, textSize, spacing);

    // Calculate centered text position
    float textX = rect.x + (rect.width - textSizeMeasure.x) / 2.0f;
    float textY = rect.y + (rect.height - textSizeMeasure.y) / 2.0f;

    // Draw centered text
    DrawTextEx(font, text, {textX, textY}, textSize, spacing, BLACK);

    // Return true if the button is clicked
    return isHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Global text input buffer
static char textInput[50];
static int letterCount = 0;

// Function to draw the input box
bool isInputActive = false;  // Track if input box is active

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

    Vector2 textPos = MeasureTextEx(codeFont, textInput, 40, 1);
    float padding = inputBox.x / 100;

    //DrawText(textInput, inputBox.x+padding, inputBox.y+(inputBox.height - textPos.y) / 2.0, 40, BLACK);
    DrawTextEx(codeFont, textInput, { inputBox.x + padding, (float)(inputBox.y + (inputBox.height - textPos.y) / 2.0) }, 40, 1, BLACK);
}

// Function to handle text input
void HandleTextInput() {
    int key = GetCharPressed();
    while (key > 0) {
        if (letterCount < 9 && key >= '0' && key <= '9') { // Valid characters
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
    float buttonHeight = screenHeight / 15;
    float padding = screenWidth / 100;

    float totalWidth = 3 * (buttonWidth + padding) + (screenWidth / 3);
    float startX = screenWidth - totalWidth - padding;
    float startY = padding;

    insertBtn = { startX, startY, buttonWidth, buttonHeight };
    deleteBtn = { startX + buttonWidth + padding, startY, buttonWidth, buttonHeight };
    searchBtn = { startX + 2 * (buttonWidth + padding), startY, buttonWidth, buttonHeight };
    inputBox = { startX + 3 * (buttonWidth + padding), startY, screenWidth / 3, buttonHeight };

     clearBtn = { padding, screenHeight - buttonHeight - padding, buttonWidth, buttonHeight };
     randomBtn = { clearBtn.x + buttonWidth + padding, clearBtn.y, buttonWidth, buttonHeight };

    loadFileBtn = { padding, randomBtn.y - buttonHeight - padding, buttonWidth, buttonHeight };

    redoBtn = { screenWidth - padding - buttonWidth, screenHeight - buttonHeight - padding, buttonWidth, buttonHeight };
    undoBtn = { redoBtn.x - buttonWidth - padding, redoBtn.y, buttonWidth, buttonHeight };
}

void DrawAndHandleButtons(const Rectangle& insertBtn, const Rectangle& deleteBtn, const Rectangle& searchBtn,
    const Rectangle& randomBtn, const Rectangle& clearBtn, const Rectangle& loadFileBtn) {
    if (DrawButton(insertBtn, "Insert", GREEN)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            open = true;
            ht.SetPendingOperation(PendingOperation::INSERT, v);
            if (runAtOnce) {
                ht.InsertInstantly(v);
            }
            else{
                ht.StartSearch(v, true); // Shows calculation
            }
            });
    }

    if (DrawButton(deleteBtn, "Delete", RED)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            ht.SetPendingOperation(PendingOperation::DELETE, v);
            open = true;
            if (runAtOnce) {
                ht.DeleteInstantly(v);
            }
            else {
                ht.StartSearch(v, false); // Shows calculation
            }
            });
    }

    if (DrawButton(searchBtn, "Search", BLUE)) {
        ProcessButtonClick([](int v) {
            ht.ResetColors();
            ht.SetPendingOperation(PendingOperation::SEARCH, v);
            open = true;
            if (runAtOnce) {
                ht.StartInstantSearch(v);
            }
            else
                ht.StartSearch(v, false); // Shows calculation
            });
    }

    if (DrawButton(randomBtn, "Random", ORANGE)) {
        ht.ResetColors();
        ht.searchMessage = "Generated random values.";
        ht.RandomInsert(50, 0, 100);
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
    if(DrawButton(undoBtn, "Previous", LIGHTGRAY)) {
        ht.ResetColors();
        ht.Undo();
    }

    if(DrawButton(redoBtn, "Next", LIGHTGRAY)) {
        ht.ResetColors();
        ht.Redo();
    }
}

bool runAtOnce = false;

void RunAtOnceBtn() {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = screenWidth / 8;
    float buttonHeight = screenHeight / 15;
    float padding = screenWidth / 100;

    Rectangle btn = { padding*2+buttonWidth, screenHeight - buttonHeight*2 - padding*2, buttonWidth, buttonHeight };
    Color color = runAtOnce ? GREEN : RED;
    if (DrawButton(btn, "Run At Once", color) ){
        runAtOnce = !runAtOnce;
    }
}

bool DrawButton2(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize) {
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

    DrawTextEx(font, text, { textX, textY }, textSize, 1.0f, WHITE);

    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)); // Return true if clicked
}

bool openUpdateBtn = false;
std::string oldValueInput = "";
std::string newValueInput = "";
bool editingOldValue = false;
bool editingNewValue = false;

void DrawUpdateBtn() {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float buttonWidth = screenWidth / 8;
    float buttonHeight = screenHeight / 15;
    float padding = screenWidth / 100;

    float totalWidth = 3 * (buttonWidth + padding) + (screenWidth / 3);
    float startX = screenWidth - totalWidth - padding;
    float startY = padding;

    Rectangle updateBtn = { startX - padding - buttonWidth, startY, buttonWidth, buttonHeight };

    if (DrawButton(updateBtn, "Update", PURPLE)) {
        openUpdateBtn = !openUpdateBtn;
        if (!openUpdateBtn) {
            oldValueInput = "";
            newValueInput = "";
            editingOldValue = false;
            editingNewValue = false;
        }
    }   

    if (openUpdateBtn) {
        float startXpos = startX - buttonWidth - 4*padding;
        float startYpos = startY + buttonHeight + padding * 1.5f;
        DrawTextEx(codeFont, "Replace", { startXpos, startYpos }, 25, 1, BLACK);
        
        Rectangle box1 = { startXpos + 6.5*padding, startYpos-0.5*padding, 130, 40 };
        Color box1Color = editingOldValue ? Fade(YELLOW, 0.3f) : WHITE;
        DrawRectangleRec(box1, box1Color);
        DrawRectangleLinesEx(box1, 1, DARKGRAY);
        DrawTextEx(codeFont, oldValueInput.c_str(), { box1.x + 5, box1.y + 5 }, 25, 1, BLACK);

        DrawTextEx(codeFont, "with", { startXpos + 15.0f * padding, startYpos }, 25, 1, BLACK);

        Rectangle box2 = { startXpos + 19.0f * padding, startYpos - 0.5f * padding, 130, 40 };
        Color box2Color = editingNewValue ? Fade(YELLOW, 0.3f) : WHITE;
        DrawRectangleRec(box2, box2Color);
        DrawRectangleLinesEx(box2, 1, DARKGRAY);
        DrawTextEx(codeFont, newValueInput.c_str(), { box2.x + 5, box2.y + 5 }, 25, 1, BLACK);

        // Handle input box selection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), box1)) {
                editingOldValue = true;
                editingNewValue = false;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), box2)) {
                editingOldValue = false;
                editingNewValue = true;
            }
            else {
                editingOldValue = false;
                editingNewValue = false;
            }
        }

        // Handle text input
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= '0' && key <= '9') ) {
                if (editingOldValue && oldValueInput.length() < 9) {
                    oldValueInput += (char)key;
                }
                else if (editingNewValue && newValueInput.length() < 9) {
                    newValueInput += (char)key;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (editingOldValue && !oldValueInput.empty()) {
                oldValueInput.pop_back();
            }
            else if (editingNewValue && !newValueInput.empty()) {
                newValueInput.pop_back();
            }
        }

        Rectangle confirmBtn = {box2.x+box2.width+padding, box2.y, box2.width, box2.height};
        if (DrawButton(confirmBtn, "Confirm", GRAY)) {
            if (oldValueInput == "" || newValueInput == "") return;
            int oldValue = std::stoi(oldValueInput);
            int newValue = std::stoi(newValueInput);
            open = true;
            if (!runAtOnce) ht.UpdateValueAnimation(oldValue, newValue);
            else ht.UpdateValueInstantly(oldValue, newValue);
        }
    }
}

bool open = false;

void DrawCodeRun() {
    int HEIGHT = GetScreenHeight(), WIDTH = GetScreenWidth();
    int w = 50, h = 280;
    Rectangle openBtn = { WIDTH - w, HEIGHT - h - 100, w, h };

    if (open) {
        if (DrawButton2(openBtn, ">", BLACK, 1, codeFont, 30)) open = false;
        if (open) {
            int width = 440, height = 40;
            int startX = WIDTH - w - 20-width;
            int startY = openBtn.y;
            const char* text[] = { "i = key % HT.length", "while (HT[i] && HT[i] -> val != key)", "   HT[i] = HT[i] -> next", "if key already exists", "", "else if key does not exist" , ""};
            const char* case1[] = { "   return found", "   do nothing", "   remove key from list", "  replace", "  do nothing"};
            const char* case2[] = { "   return not found", "   insert key to the head of list", "   do nothing", "  do nothing", "  check value want to replace"};
            for (int i = 0; i < 7; i++) {
                Rectangle rect = { startX, startY +height*i , width, height };
                DrawButton2(rect, "", BLACK, 2, codeFont, 20);

                if (i == 4) {
                    if (ht.GetPendingOperation() == PendingOperation::SEARCH) {
                        DrawButton2(rect, case1[0], (state!=4)?BLACK:GRAY, 2, codeFont, 20);
                    }
                    if (ht.GetPendingOperation() == PendingOperation::INSERT ) {
                        DrawButton2(rect, case1[1], (state!=4)?BLACK:GRAY, 2, codeFont, 20);
                    }
                    else if(ht.GetPendingOperation() == PendingOperation::DELETE) {
                        DrawButton2(rect, case1[2], (state != 4) ? BLACK : GRAY, 2, codeFont, 20);
                    }
                    else if (ht.GetPendingOperation() == PendingOperation::UPDATE) {
                        DrawButton2(rect, case1[3], (state != 4) ? BLACK : GRAY, 2, codeFont, 20);
                    }
                    else if (ht.GetPendingOperation() == PendingOperation::CHECK_NEW_VALUE) {
                        DrawButton2(rect, case1[4], (state != 4) ? BLACK : GRAY, 2, codeFont, 20);
                    }
                }
                else if (i == 6) {
                    if (ht.GetPendingOperation() == PendingOperation::SEARCH) {
                        DrawButton2(rect, case2[0], (state!=6)?BLACK:GRAY, 2, codeFont, 20);
                    }
                    if ((ht.GetPendingOperation() == PendingOperation::INSERT) ){
                        DrawButton2(rect, case2[1], (state!=6)?BLACK:GRAY, 2, codeFont, 20);
                    }
                    else if ((ht.GetPendingOperation() == PendingOperation::DELETE)) {
                        DrawButton2(rect, case2[2], (state!=6)?BLACK:GRAY, 2, codeFont, 20);
                    }
                    else if (ht.GetPendingOperation() == PendingOperation::UPDATE) {
                        DrawButton2(rect, case2[3], (state != 6) ? BLACK : GRAY, 2, codeFont, 20);
                    }
                    else if (ht.GetPendingOperation() == PendingOperation::CHECK_NEW_VALUE) {
                        DrawButton2(rect, case2[4], (state != 6) ? BLACK : GRAY, 2, codeFont, 20);
                    }
                }
                else if (i == 5) {
                    DrawButton2(rect, text[i], (state != 6) ? BLACK : GRAY, 2, codeFont, 20);
                }
                else if (i == 3) {
                    DrawButton2(rect, text[i], (state != 4) ? BLACK : GRAY, 2, codeFont, 20);

                }
                else DrawButton2(rect, text[i], (state!=i)?BLACK:GRAY, 2, codeFont, 20);
            }
        }
    }
    else {
        if (DrawButton2(openBtn, "<", BLACK, 1, codeFont, 20)) open = true;
    }
}

void DrawHashTable() {
    Rectangle insertBtn, deleteBtn, searchBtn, inputBox, randomBtn, clearBtn, undoBtn, redoBtn, loadFileBtn;
    DefineUIElements(insertBtn, deleteBtn, searchBtn, inputBox, randomBtn, clearBtn, undoBtn, redoBtn, loadFileBtn);

    DrawAndHandleButtons(insertBtn, deleteBtn, searchBtn, randomBtn, clearBtn, loadFileBtn);
    DrawUndoRedoButtons(undoBtn, redoBtn);
    RunAtOnceBtn();
    DrawUpdateBtn();

    ht.HandleTableDragging();
    ht.UpdateDeletionAnimation();
    ht.UpdateSearchAnimation();
    ht.Draw();

    DrawInputBox(inputBox);
    HandleTextInput();

    DrawCodeRun();
}