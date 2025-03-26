#include "AVLtreeAnimation.h"
#include "Color.h"
#include <sstream>
#include <cstdlib>
#include <stack>
#include <iostream>
#include <cmath>

std::stack<AVLTree> treeUndoState;
std::stack<AVLTree> treeRedoState;
#define NODE_RADIUS 20.0f

AVLTreeVisualizer::AVLTreeVisualizer() {
    inputText = "";
    inputActive = false;
    handleSpace = { 0, 0, 1600, 75 };
    inputBox     = { 20, 20, 200, 30 };
    insertButton = { 230, 20, 80, 30 };
    deleteButton = { 320, 20, 80, 30 };
    searchButton = { 410, 20, 90, 30 };
    randomButton = { 510, 20, 90, 30 };
    clearButton  = { 610, 20, 70, 30 };
    loadFileButton = {690, 20, 110, 30};
    previousButton = { 1000, 20, 105, 30 };
    nextButton = { 1120, 20, 65, 30 }; 

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

void AVLTreeVisualizer::drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight) {
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
        float hypotenus = sqrt(60.0f * 60.0f + leftX * leftX);
        float startLineX = x - NODE_RADIUS*leftX/hypotenus;
        float startLineY = y + NODE_RADIUS*60.0f/hypotenus;
        DrawLine(startLineX, startLineY, x - leftX, y + 60, BLACK);
        drawTree(node->left, x - leftX, y + 60, offset / 2, highlight);
    }
    if (node->right) {
        float rightX = tree.getSubtreeWidth(node->right->left);
        float hypotenus = sqrt(60.0f * 60.0f + rightX * rightX);
        float startLineX = x + NODE_RADIUS * rightX / hypotenus;
        float startLineY = y + NODE_RADIUS * 60.0f / hypotenus;
        DrawLine(startLineX, startLineY, x + rightX, y + 60, BLACK);
        drawTree(node->right, x + rightX, y + 60, offset / 2, highlight);
    }
}

//draw button
void AVLTreeVisualizer::draw() {
    std::set<Node*> highlightNodes(currentPath.begin(), currentPath.begin() + pathIndex);

    DrawRectangleRec(handleSpace, HandleInputSpaceBG);
    DrawRectangleRec(inputBox, LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, inputActive ? BLUE : GRAY);
    DrawText(inputText.c_str(), inputBox.x + 5, inputBox.y + 5, 20, BLACK);

    DrawRectangleRec(insertButton, GREEN);
    DrawRectangleRec(deleteButton, ORANGE);
    DrawRectangleRec(searchButton, BLUE);
    DrawRectangleRec(randomButton, PURPLE);
    DrawRectangleRec(clearButton, RED);
    DrawRectangleRec(loadFileButton, MY_TURQUOIS);
    DrawRectangleRec(previousButton, BlueButton);
    DrawRectangleRec(nextButton, BlueButton);

    DrawText("Insert", insertButton.x + 10, insertButton.y + 5, 20, WHITE);
    DrawText("Delete", deleteButton.x + 10, deleteButton.y + 5, 20, WHITE);
    DrawText("Search", searchButton.x + 10, searchButton.y + 5, 20, WHITE);
    DrawText("Random", randomButton.x + 10, randomButton.y + 5, 20, WHITE);
    DrawText("Clear", clearButton.x + 10, clearButton.y + 5, 20, WHITE);
    DrawText("Load File", loadFileButton.x + 10, loadFileButton.y + 5, 20, WHITE);
    DrawText("Previous", previousButton.x + 10, previousButton.y + 5, 20, WHITE);
    DrawText("Next", nextButton.x + 10, nextButton.y + 5, 20, WHITE);

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
    std::cout << treeUndoState.size() << std::endl;

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

void AVLTreeVisualizer::animateLoadFile(){

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