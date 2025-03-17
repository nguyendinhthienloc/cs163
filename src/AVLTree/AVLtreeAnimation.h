
#include "AVLtree.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <set>

class AVLTreeVisualizer {
public: 
    AVLTree tree;
    std::string inputText;
    bool inputActive;
    Rectangle inputBox;
    Rectangle insertButton;
    Rectangle deleteButton;
    Rectangle searchButton;
    Rectangle randomButton;

    enum AnimationState { IDLE, TRAVERSING, INSERTING, ROTATING, SHOWING_RESULT };
    AnimationState currentState;
    std::string currentOperation; // "insert", "delete", "search"
    std::vector<Node*> currentPath;
    int pathIndex;
    std::vector<Node*> rotationNodes;
    int rotationIndex;
    bool searchFound;
    float stateTimer;
    float resultTimer;
    int operationValue;
    int pendingInsertValue;

    void drawTree(Node* node, float x, float y, float offset, const std::set<Node*>& highlight);

    AVLTreeVisualizer();
    void handleInput();
    void updateAnimation(float deltaTime);
    void draw();
    void animateInsert(int value);
    void animateDelete(int value);
    void animateSearch(int value);
    void animateRandom();
};

