#include "AVLtreeAnimation.h"
#include <sstream>
#include <cstdlib>

AVLTreeVisualizer::AVLTreeVisualizer() {
    inputText = "";
    inputActive = false;
    inputBox = { 20, 20, 200, 30 };
    insertButton = { 230, 20, 80, 30 };
    deleteButton = { 320, 20, 80, 30 };
    searchButton = { 410, 20, 80, 30 };
    randomButton = { 500, 20, 80, 30 };

    currentState = IDLE;
    pathIndex = 0;
    rotationIndex = 0;
    searchFound = false;
    stateTimer = 0.0f;
    resultTimer = 0.0f;
    operationValue = 0;
    pendingInsertValue = 0;
}



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
                currentState = INSERTING; // Move to insertion state
            }
            else {
                currentState = SHOWING_RESULT;
                resultTimer = 0.0f;
            }
            stateTimer = 0.0f;
        }
        break;

    case INSERTING:
        if (stateTimer >= 0.5f) { // Give a brief pause before insertion
            tree.insert(tree.root, pendingInsertValue); // Now perform the insertion
            currentState = SHOWING_RESULT;
            resultTimer = 0.0f;
        }
        break;

    case ROTATING:
        // Note: Without explicit rotation tracking, we'll skip this for now
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
        }
        break;
    }
}

void AVLTreeVisualizer::drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight) {
    if (!node) return;

    Color nodeColor = (highlight.count(node)) ? YELLOW : WHITE;
    if (currentState == SHOWING_RESULT && node->data == operationValue) {
        nodeColor = (currentOperation == "search" && !searchFound) ? RED : GREEN;
    }

    DrawCircle(x, y, 20, nodeColor);
    std::string valueStr = std::to_string(node->data);
    DrawText(valueStr.c_str(), x - MeasureText(valueStr.c_str(), 20) / 2, y - 10, 20, BLACK);

    if (node->left) {
        DrawLine(x, y, x - offset, y + 60, WHITE);
        drawTree(node->left, x - offset, y + 60, offset / 2, highlight);
    }
    if (node->right) {
        DrawLine(x, y, x + offset, y + 60, WHITE);
        drawTree(node->right, x + offset, y + 60, offset / 2, highlight);
    }
}

void AVLTreeVisualizer::draw() {
    std::set<Node*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);

    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawText(inputText.c_str(), inputBox.x + 5, inputBox.y + 5, 20, BLACK);

    DrawRectangleRec(insertButton, GREEN);
    DrawRectangleRec(deleteButton, RED);
    DrawRectangleRec(searchButton, BLUE);
    DrawRectangleRec(randomButton, PURPLE);

    DrawText("Insert", insertButton.x + 10, insertButton.y + 5, 20, WHITE);
    DrawText("Delete", deleteButton.x + 10, deleteButton.y + 5, 20, WHITE);
    DrawText("Search", searchButton.x + 10, searchButton.y + 5, 20, WHITE);
    DrawText("Random", randomButton.x + 10, randomButton.y + 5, 20, WHITE);

    if (tree.root) {
        drawTree(tree.root, GetScreenWidth() / 2, 100, 200, highlightNodes);
    }
}

void AVLTreeVisualizer::animateInsert(int value) {
    currentOperation = "insert";
    operationValue = value;
    pendingInsertValue = value; // Store the value for later insertion
    currentPath = tree.getInsertionPath(value); // Get path based on current tree
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
}

void AVLTreeVisualizer::animateSearch(int value) {
    currentOperation = "search";
    operationValue = value;
    currentPath = tree.getInsertionPath(value);
    pathIndex = 0;
    currentState = TRAVERSING;
    searchFound = tree.search(value);
}

void AVLTreeVisualizer::animateRandom() {
    int* arrOfRandomInt =  LoadRandomSequence(15, 0, 100);
    
    tree.destroyTree(tree.root);
    tree.root = nullptr;
    for (int i = 0; i < 15; i++) {
        tree.insert(tree.root, arrOfRandomInt[i]);
    }
    delete[] arrOfRandomInt;
}