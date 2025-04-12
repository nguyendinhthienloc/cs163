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

AVLTreeVisualizer::AVLTreeVisualizer() {
    inputText = "";
    inputActive = false;
    handleSpace = { 0, 0, 1600, 75 };
    inputBox     = { 170, 20, 200, 40 };
    insertButton = { 380, 20, 80, 40 };
    deleteButton = { 470, 20, 80, 40 };
    searchButton = { 560, 20, 90, 40 };
    randomButton = { 660, 20, 90, 40 };
    clearButton  = { 760, 20, 70, 40 };
    loadFileButton = {840, 20, 110, 40};
    previousButton = { 1000, 20, 105, 40 };
    nextButton = { 1120, 20, 65, 40 }; 

    currentState = IDLE;
    pathIndex = 0;
    rotationIndex = 0;
    searchFound = false;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    operationValue = 0;
    pendingInsertValue = 0;
}

//button click
void AVLTreeVisualizer::handleInput() {
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        inputActive = false;
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

    if (CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        animateInsert(std::stoi(inputText));
        inputText.clear();
    }
    if (CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        animateDelete(std::stoi(inputText));
        inputText.clear();
    }
    if (CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inputText.empty()) {
        animateSearch(std::stoi(inputText));
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
}

//timing algorithms
void AVLTreeVisualizer::updateAnimation(float deltaTime) {
    if (currentState == IDLE) {
        return;
    }

    stateTimer += deltaTime;

    switch (currentState) {
    case TRAVERSING:
        if (stateTimer >= 0.5f && pathIndex < currentPath.size()) {
            pathIndex++;
            stateTimer = 0.0f;
        }
        if (pathIndex >= currentPath.size()) {
            if (currentOperation == "insert") {
                currentState = INSERTING; 
            }
            else {
                currentState = SHOWING_RESULT; 
            }
            stateTimer = 0.0f;
        }
        break;

    case INSERTING:
        if (stateTimer >= 0.5f) {
            tree.insert(tree.root, pendingInsertValue); 
            AVLTree treeReplica(tree);
            treeUndoState.push(treeReplica);
            currentPath = tree.getInsertionPath(pendingInsertValue); 
            pathIndex = currentPath.size(); 
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;
            stateTimer = 0.0f;
        }
        break;

    case ROTATING:
        currentState = SHOWING_RESULT;
        resultTimer = 0.0f;
        break;

    case SHOWING_RESULT:
        resultTimer += deltaTime;
        if (resultTimer >= 2.0f) {
            currentState = IDLE;
            currentPath.clear();
            rotationNodes.clear();
            pathIndex = 0;
            rotationIndex = 0;
            searchFound = false; // Reset searchFound after showing result
        }
        break;
    }
}

void AVLTreeVisualizer::drawTree(AVLNode* node, float x, float y, float offset, const std::set<AVLNode*>& highlight) {
    if (!node) return;

    Color nodeColor = (highlight.count(node)) ? YELLOW : WHITE;
    if (currentState == SHOWING_RESULT && node->data == operationValue) {

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
        drawTree(node->left, x - leftX, y + 90, offset / 2, highlight);
    }
    if (node->right) {
        float rightX = tree.getSubtreeWidth(node->right->left);
        float hypotenus = sqrt(90.0f * 90.0f + rightX * rightX);
        float startLineX = x + NODE_RADIUS * rightX / hypotenus;
        float startLineY = y + NODE_RADIUS * 90.0f / hypotenus;
        DrawLine(startLineX, startLineY, x + rightX, y + 90, BLACK);
        drawTree(node->right, x + rightX, y + 90, offset / 2, highlight);
    }
}

void AVLTreeVisualizer::drawButton(Rectangle rect, const char* text, Color color) {
	Vector2 mousePos = GetMousePosition();
	bool isHover = CheckCollisionPointRec(mousePos, rect);

	DrawRectangleRec(rect, isHover ? Fade(color, 0.7f) : color);
	DrawRectangleLinesEx(rect, 2, BLACK);

	Vector2 textSize = MeasureTextEx(GetFontDefault(), text, 20, 2);
	float textX = rect.x + (rect.width - textSize.x) / 2;
	float textY = rect.y + (rect.height - textSize.y) / 2;
	DrawText(text, textX, textY, 20, WHITE);
}

//draw button
void AVLTreeVisualizer::draw() {
    std::set<AVLNode*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);

    DrawRectangleRec(handleSpace, HandleInputSpaceBG);
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawText(inputText.c_str(), inputBox.x + 5, inputBox.y + 10, 20, BLACK);


	drawButton(insertButton, "Insert", GREEN);
	drawButton(deleteButton, "Delete", RED);
	drawButton(searchButton, "Search", BLUE);
	drawButton(randomButton, "Random", ORANGE);
	drawButton(clearButton, "Clear", RED);
	drawButton(loadFileButton, "Load File", PURPLE);
	drawButton(previousButton, "Previous", BlueButton);
	drawButton(nextButton, "Next", BlueButton);

    if (tree.root) {
        drawTree(tree.root, GetScreenWidth() / 2, 120, 200, highlightNodes);
    }

    if (currentState == SHOWING_RESULT && currentOperation == "search" && !searchFound) {
        std::string messageNotFound = std::to_string(operationValue) + " is not in the tree";
        int messageSize = MeasureText(messageNotFound.c_str(), 20);
        DrawText(messageNotFound.c_str(), (GetScreenWidth() - messageSize) / 2, GetScreenHeight() - 30, 20, RED);
    }
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
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    currentState = TRAVERSING;
    tree.remove(tree.root, value);
    AVLTree treeReplica(tree);
    treeUndoState.push(treeReplica);
}

void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;
    currentPath = tree.getInsertionPath(value); 
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
