#include <sstream>
#include <cstdlib>
#include <stack>
#include <cmath>
#include "../../header/AVL Tree/AVLtreeAnimation.h"
#include "../../header/Color.h"
#include "../../resource/tinyfiledialogs.h"

std::stack<AVLTree> treeUndoState;
std::stack<AVLTree> treeRedoState;
#define NODE_RADIUS 40.0f
int option = 0;

AVLTreeVisualizer::AVLTreeVisualizer() {
    inputText = "";
    inputActive = false;
    isDragging = false;
    dragOffset = { 0, 0 };
    dragStartPos = { 0, 0 };
    handleSpace = { 0, 0, 1600, 80 };
    inputBox     = { 170, 20, 200, 40 };
    insertButton = { 380, 20, 80, 40 };
    deleteButton = { 470, 20, 80, 40 };
    searchButton = { 560, 20, 90, 40 };
    randomButton = { 660, 20, 90, 40 };
    clearButton  = { 760, 20, 70, 40 };
    loadFileButton = {840, 20, 110, 40};
    previousButton = { 1000, 20, 110, 40 };
    nextButton = { 1120, 20, 110, 40 }; 
	stdViewButton = { 1240, 20, 110, 40 };
    runAtOnceButton = { 1360, 20, 150, 40 };

    currentState = IDLE;
    pathIndex = 0;
    rotationIndex = 0;
    searchFound = false;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    operationValue = 0;
    pendingInsertValue = 0;
	isRunAtOnce = false;
    duplicateFound = false; // Initialize new flag
}

//button click logic
void AVLTreeVisualizer::handleInput() {
    Vector2 mousePos = GetMousePosition();

    bool isOverUI = CheckCollisionPointRec(mousePos, inputBox) ||
        CheckCollisionPointRec(mousePos, insertButton) ||
        CheckCollisionPointRec(mousePos, deleteButton) ||
        CheckCollisionPointRec(mousePos, searchButton) ||
        CheckCollisionPointRec(mousePos, randomButton) ||
        CheckCollisionPointRec(mousePos, clearButton) ||
        CheckCollisionPointRec(mousePos, loadFileButton) ||
        CheckCollisionPointRec(mousePos, previousButton) ||
        CheckCollisionPointRec(mousePos, nextButton) ||
        CheckCollisionPointRec(mousePos, stdViewButton) ||
        CheckCollisionPointRec(mousePos, runAtOnceButton) ||
        CheckCollisionPointRec(mousePos, { 20, 20, 100, 40 });

    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isOverUI) {
        // Start dragging if not over UI
        inputActive = false;
        isDragging = true;
        dragStartPos = mousePos;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isDragging = false;
    }

    if (isDragging) {
        dragOffset.x += mousePos.x - dragStartPos.x;
        dragOffset.y += mousePos.y - dragStartPos.y;
        dragStartPos = mousePos;
    }

    if (inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48 && key <= 57) && inputText.length() < 6) {
                inputText += (char)key;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
            inputText.pop_back();
        }
    }

    if (CheckCollisionPointRec(mousePos, runAtOnceButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		isRunAtOnce = !isRunAtOnce; // Toggle the run at once mode
    }

    if (CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
		if (!isRunAtOnce) {
			animateInsert(std::stoi(inputText));
		}
		else {
            AVLTree treeReplica(tree);
            treeUndoState.push(treeReplica);
			while (!treeRedoState.empty()) {
				treeRedoState.pop();
			}
            currentOperation = "insert";
            currentState = SHOWING_RESULT;
            codeState = tree.search(std::stoi(inputText)) ? 6 : 1;
			tree.insert(tree.root, std::stoi(inputText));
            resultTimer = 0.0f;
            stateTimer = 0.0f;
            currentPath.clear();
            pathIndex = 0;
            
		}
        inputText.clear();       
    }

    if (CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        if (!isRunAtOnce) {
            animateDelete(std::stoi(inputText));
        }
        else {
            AVLTree treeReplica(tree);
            treeUndoState.push(treeReplica);
            while (!treeRedoState.empty()) {
                treeRedoState.pop();
            }
            currentOperation = "delete";
            operationValue = std::stoi(inputText);
            currentState = SHOWING_RESULT;
            codeState = tree.search(std::stoi(inputText)) ? 1 : 6;
            tree.remove(tree.root, std::stoi(inputText));
            resultTimer = 0.0f;
            stateTimer = 0.0f;
            currentPath.clear();
            pathIndex = 0;

        }
        inputText.clear();
    }
    
	
    if (CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        int value = std::stoi(inputText);
        if (!isRunAtOnce) {
            animateSearch(value); 
        }
        else {
            currentOperation = "search";
            operationValue = value;
            searchFound = tree.search(value);
            codeState = searchFound ? 1 : 6;
            currentState = SHOWING_RESULT; 
            resultTimer = 0.0f; 
            stateTimer = 0.0f;
            currentPath.clear();
            pathIndex = 0;
        }
        inputText.clear();
    }
    if (CheckCollisionPointRec(mousePos, randomButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateRandom();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, clearButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateClear();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, loadFileButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateLoadFile();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, previousButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animatePrevious();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, nextButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        animateNext();
        inputText.clear();
    }

    if (CheckCollisionPointRec(mousePos, stdViewButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        dragOffset = { 0, 0 };
    }
}

//timing algorithms
void AVLTreeVisualizer::updateAnimation(float deltaTime) {
    if (currentState == IDLE) {
        codeState = -1; // No code highlighted when idle
        duplicateFound = false; // Reset duplicate flag
        return;
    }

    stateTimer += deltaTime;

    switch (currentState) {
    case TRAVERSING:
        if (currentOperation == "insert") {
            if (pathIndex < currentPath.size()) {
                AVLNode* currentNode = currentPath[pathIndex];
                if (stateTimer >= 0.5f) {
                    // Check the comparison at the current node
                    if (pendingInsertValue < currentNode->data) {
                        codeState = 2; // "if value to be inserted < this key"
                    }
                    else if (pendingInsertValue > currentNode->data) {
                        codeState = 4; // "else if value to be inserted > this key"
                    }
                    else {
                        codeState = 6; // "else do nothing"
                        duplicateFound = true;
                    }
                    pathIndex++;
                    stateTimer = 0.0f;
                }
                // After incrementing pathIndex, set the traversal direction
                if (pathIndex < currentPath.size()) {
                    if (pendingInsertValue < currentNode->data) {
                        codeState = 3; // "go left"
                    }
                    else if (pendingInsertValue > currentNode->data) {
                        codeState = 5; // "go right"
                    }
                }
                else if (!duplicateFound) {
                    codeState = 0; // "if insertion point is found"
                }
            }
            if (pathIndex >= currentPath.size()) {
                if (currentOperation == "insert") {
                    if (duplicateFound) {
                        currentState = SHOWING_RESULT; // Skip insertion for duplicates
                        codeState = 6; // Keep "else do nothing" highlighted
                    }
                    else {
                        currentState = INSERTING;
                        codeState = 1; // "create new vertex"
                    }
                }
                else {
                    currentState = SHOWING_RESULT;
                }
                stateTimer = 0.0f;
            }
        }

        else if (currentOperation == "delete" || currentOperation == "search") {
            if (pathIndex < currentPath.size()) {
                AVLNode* currentNode = currentPath[pathIndex];
                if (stateTimer >= 0.5f) {
                    // Check the comparison at the current node
                    if (pendingInsertValue < currentNode->data) {
                        codeState = 2; // "if value to be inserted < this key"
                    }
                    else if (pendingInsertValue > currentNode->data) {
                        codeState = 4; // "else if value to be inserted > this key"
                    }
                    else {
                        codeState = 1; 
                        duplicateFound = true;
                    }
                    pathIndex++;
                    stateTimer = 0.0f;
                }
                // After incrementing pathIndex, set the traversal direction
                if (pathIndex < currentPath.size()) {
                    if (pendingInsertValue < currentNode->data) {
                        codeState = 3; // "go left"
                    }
                    else if (pendingInsertValue > currentNode->data) {
                        codeState = 5; // "go right"
                    }
                }
                else if (!duplicateFound) {
                    codeState = 6; // "else do nothing"
                }
            }
            if (pathIndex >= currentPath.size()) {
                if (currentOperation == "delete") {
                    if (!duplicateFound) {
                        currentState = SHOWING_RESULT; 
                        codeState = 6; // Keep "else do nothing" highlighted
                    }
                    else {
                        currentState = DELETING;
                        codeState = 1; // "delete this vertex"
                        //duplicateFound = false;
                    }
                }

                else if (currentOperation == "search") {
                    if (!tree.search(operationValue)) {
                        currentState = SHOWING_RESULT;
                        codeState = 6; // Keep "else do nothing" highlighted
                    }
                    else {
                        currentState = SHOWING_RESULT;
                        codeState = 1; // "highligh vertex"
                    }
                }
                else {
                    currentState = SHOWING_RESULT;
                }
                stateTimer = 0.0f;
            }
        }

        break;

    case INSERTING:
        if (stateTimer >= 0.5f) {
            tree.insert(tree.root, pendingInsertValue);
            AVLTree treeReplica(tree);
            treeUndoState.push(treeReplica);
            while (!treeRedoState.empty()) {
                treeRedoState.pop();
            }
            currentPath = tree.getInsertionPath(pendingInsertValue);
            pathIndex = currentPath.size();
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;
            stateTimer = 0.0f;
            duplicateFound = false; // Reset duplicate flag
        }
        break;

    case DELETING:
        if (stateTimer >= 0.5f) {
            AVLTree treeReplica(tree);
            treeUndoState.push(treeReplica);
            while (!treeRedoState.empty()) {
                treeRedoState.pop();
            }
            tree.remove(tree.root, operationValue);
            currentPath.clear();
            pathIndex = 0;
            /*currentPath = tree.getInsertionPath(pendingInsertValue);
            pathIndex = currentPath.size();*/
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;
            stateTimer = 0.0f;
            duplicateFound = false;
        }
        break;

    case ROTATING:
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        break;

    case SHOWING_RESULT:
        //resultTimer += deltaTime;
        if (resultTimer >= 2.0f) {
            currentState = IDLE;
            /*currentPath.clear();*/
            rotationNodes.clear();
            pathIndex = 0;
            rotationIndex = 0;
            searchFound = false;
            duplicateFound = false; // Reset duplicate flag
        }
        break;
    }
}

void AVLTreeVisualizer::drawTree(AVLNode* node, float x, float y, const std::set<AVLNode*>& highlight) {
    if (!node) return;

    Color nodeColor = (highlight.count(node)) ? GOLD : WHITE;
    if (currentState == DELETING && node->data == operationValue) {
        nodeColor = RED; // Highlight the node being deleted
    }
    else if (currentState == SHOWING_RESULT && node->data == operationValue) {
        if (currentOperation == "search" && searchFound)
            nodeColor = GREEN;
    }

    DrawCircle(x, y, NODE_RADIUS, nodeColor);
    std::string valueStr = std::to_string(node->data);
    DrawText(valueStr.c_str(), x - MeasureText(valueStr.c_str(), 20) / 2, y - 10, 20, BLACK);

    if (node->left) {
        float leftX = tree.getSubtreeWidth(node->left->right);
        float hypotenus = sqrt(90.0f * 90.0f + leftX * leftX);
        float startLineX = x - NODE_RADIUS*leftX/hypotenus;
        float startLineY = y + NODE_RADIUS*90.0f/hypotenus;
        DrawLine(startLineX, startLineY, x - leftX, y + 90, BLACK);
        drawTree(node->left, x - leftX, y + 90, highlight);
    }
    if (node->right) {
        float rightX = tree.getSubtreeWidth(node->right->left);
        float hypotenus = sqrt(90.0f * 90.0f + rightX * rightX);
        float startLineX = x + NODE_RADIUS * rightX / hypotenus;
        float startLineY = y + NODE_RADIUS * 90.0f / hypotenus;
        DrawLine(startLineX, startLineY, x + rightX, y + 90, BLACK);
        drawTree(node->right, x + rightX, y + 90, highlight);
    }
}

void AVLTreeVisualizer::drawButton(Rectangle rect, const char* text, Color color) {
	Vector2 mousePos = GetMousePosition();
	bool isHover = CheckCollisionPointRec(mousePos, rect);

	DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
	DrawRectangleLinesEx(rect, 2, BLACK);

	Vector2 textSize = MeasureTextEx(codeFont, text, 20, 2);
	float textX = rect.x + (rect.width - textSize.x) / 2.0f+2;
	float textY = rect.y + (rect.height - textSize.y) / 2.0f;
    DrawTextEx(codeFont, text, { textX, textY }, 20, 1, WHITE);
}

bool isCodeOpen_ = false;

bool DrawButton2_(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize) {
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

int codeState = -1;

void AVLTreeVisualizer::DrawCodeBox() {
    int HEIGHT = GetScreenHeight(), WIDTH = GetScreenWidth();
    int w = 50, h = 280;
    Rectangle openBtn = { WIDTH - w, HEIGHT - h-10, w, h };

    if(DrawButton2_(openBtn, "<", BlueButton, 1, codeFont, 30)) isCodeOpen_=!isCodeOpen_;

    if (isCodeOpen_) {
        int width = 470, height = 40;
        int startX = WIDTH - w - 10 - width;
        int startY = openBtn.y;

        const char* text[3][7] = { {"if insertion point is found", "  create new vertex", "if value to be inserted < this key",
                                "  go left", "else if value to be inserted > this key", "  go right", "else do nothing" },
                                                       {"if deletion point is found", "  delete this vertex", "if value to be inserted < this key",
                                "  go left", "else if value to be inserted > this key", "  go right", "else do nothing" },
                                                       {"if search point is found", "  highlight vertex", "if value to be inserted < this key",
                                "  go left", "else if value to be inserted > this key", "  go right", "else do nothing" } };

        
        if (currentOperation == "insert") {
            option = 0;
        }
		else if (currentOperation == "delete") {
			option = 1;
		}
		else if (currentOperation == "search") {
			option = 2;
		}

        for (int i = 0; i < 7; i++) {
            Rectangle rect = { startX, startY + height * i , width, height };
            Color codeBoxColor = BlueButton;
            if (codeState == i) {
                codeBoxColor = Fade(codeBoxColor, 0.7f);
            }

            DrawButton2_(rect, text[option][i], codeBoxColor, 2, codeFont, 20);
        }
    }
}

//buttons and message are drawn here
void AVLTreeVisualizer::draw() {
    std::set<AVLNode*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);

    DrawRectangleRec(handleSpace, HandleInputSpaceBG);
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawTextEx(codeFont, inputText.c_str(), { inputBox.x + 5, inputBox.y + 10 }, 20, 1, BLACK);


	drawButton(insertButton, "Insert", GREEN);
	drawButton(deleteButton, "Delete", RED);
	drawButton(searchButton, "Search", BLUE);
	drawButton(randomButton, "Random", ORANGE);
	drawButton(clearButton, "Clear", RED);
	drawButton(loadFileButton, "Load File", PURPLE);
	drawButton(previousButton, "Previous", BlueButton);
	drawButton(nextButton, "Next", BlueButton);
    drawButton(stdViewButton, "Std View", BlueButton);
    drawButton(runAtOnceButton, "Run at Once", isRunAtOnce? GREEN : RED);
    

    if (tree.root) {
        drawTree(tree.root, GetScreenWidth() / 2 + dragOffset.x, 130 + dragOffset.y, highlightNodes);
    }

    if (currentState == SHOWING_RESULT && currentOperation == "search") {
        if (searchFound) {
            std::string messageFound = std::to_string(operationValue) + " is in the tree";
            int messageSize = MeasureText(messageFound.c_str(), 40);
            DrawTextEx(codeFont, messageFound.c_str(), { (GetScreenWidth() - messageSize) / 2.0f, (float)(GetScreenHeight() - 50) }, 40, 1, PURPLE);
        }
        else {
            std::string messageNotFound = std::to_string(operationValue) + " is not in the tree";
            int messageSize = MeasureText(messageNotFound.c_str(), 40);
            DrawTextEx(codeFont, messageNotFound.c_str(), { (GetScreenWidth() - messageSize) / 2.0f, (float)(GetScreenHeight() - 50) }, 40, 1, RED);
        }
    }

    DrawCodeBox();
}

void AVLTreeVisualizer::animateInsert(int value) {
    currentOperation = "insert";
    operationValue = value;
    pendingInsertValue = value;
    currentPath = tree.getInsertionPath(value); 
    pathIndex = 0;
    currentState = TRAVERSING;
}

void AVLTreeVisualizer::animateDelete(int value) {
    currentOperation = "delete";
    operationValue = value;
    pendingInsertValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    currentState = TRAVERSING;
    
}

void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;
    currentPath = tree.getInsertionPath(value); 
    pendingInsertValue = value;
    pathIndex = 0;
    searchFound = tree.search(value); 
    currentState = TRAVERSING;
    stateTimer = 0.0f; 
    resultTimer = 0.0f; 
}

void AVLTreeVisualizer::animateRandom() {
    // Destroy the existing tree
    tree.~AVLTree();
    new (&tree) AVLTree(); 

    int* randomValues = LoadRandomSequence(15, 1, 100);

    for (int i = 0; i < 15; i++) {
        tree.insert(tree.root, randomValues[i]);
    }

    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);

    UnloadRandomSequence(randomValues);

    currentOperation = "random";
    currentPath.clear();
    pathIndex = 0;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::animateClear() {
    tree.~AVLTree();
    new (&tree) AVLTree();

    currentOperation = "";
    currentPath.clear();
    pathIndex = 0;
    currentState = IDLE;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
	while (!treeUndoState.empty()) {
		treeUndoState.pop();
	}
	while (!treeRedoState.empty()) {
		treeRedoState.pop();
	}
}

void AVLTreeVisualizer::animateLoadFile() {
    const char* filters[] = { "*.txt" };
    const char* filePath = tinyfd_openFileDialog(
        "Select a Text File", "", 1, filters, "Text Files", 0
    );

    if (!filePath) {
        currentOperation = "loadfile";
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideFilePath = converter.from_bytes(filePath);

    // Use fopen_s instead of fopen
    FILE* file = nullptr;
    errno_t err = _wfopen_s(&file, wideFilePath.c_str(), L"r");
    if (err != 0 || !file) {
        currentOperation = "loadfile";
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        return;
    }

    // Clear the current tree before loading new data
    tree.~AVLTree();
    new (&tree) AVLTree();

    int value;
    while (fscanf_s(file, "%d", &value) == 1) {
        tree.insert(tree.root, value);
    }

    fclose(file);

    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);

    currentOperation = "loadfile";
    currentPath.clear();
    pathIndex = 0;
    currentState = SHOWING_RESULT;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
}

void AVLTreeVisualizer::animatePrevious() {
    if (!treeUndoState.empty()) {
        
        //push the current tree to redo stack
        AVLTree treeReplica(tree);
        treeRedoState.push(treeReplica);

        //change the tree to previous state
        tree = treeUndoState.top();
        treeUndoState.pop();

        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;
    }
}

void AVLTreeVisualizer::animateNext() {
    if (!treeRedoState.empty()) {
        //push the current tree to undo stack
        AVLTree treeReplica(tree);
        treeUndoState.push(treeReplica);

        //change the tree to next state
        tree = treeRedoState.top();
        treeRedoState.pop();

        currentOperation = "";
        currentPath.clear();
        pathIndex = 0;
        currentState = IDLE;
        stateTimer = 0.0f;
        resultTimer = 0.0f;
    }
}
