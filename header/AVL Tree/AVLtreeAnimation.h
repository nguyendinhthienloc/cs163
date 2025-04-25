#include "AVLtree.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include "../../resource/tinyfiledialogs.h"
#include "../Font.h"
#include <locale>
#include <codecvt>

extern int codeState;

class AVLTreeVisualizer {
public: 
    AVLTree tree;
    std::string inputText;
    bool inputActive, isDragging, duplicateFound;;
    Vector2 dragOffset;
    Vector2 dragStartPos;
    Rectangle handleSpace;
    Rectangle inputBox;
    Rectangle insertButton;
    Rectangle deleteButton;
    Rectangle searchButton;
    Rectangle randomButton;
    Rectangle clearButton;
    Rectangle loadFileButton;
    Rectangle previousButton;
    Rectangle nextButton;
	Rectangle stdViewButton;
	Rectangle runAtOnceButton;

    enum AnimationState { IDLE, TRAVERSING, INSERTING, DELETING, ROTATING, SHOWING_RESULT };
    AnimationState currentState;
    std::string currentOperation; // "insert", "delete", "search"
    std::vector<AVLNode*> currentPath;
    int pathIndex;
    std::vector<AVLNode*> rotationNodes;
    int rotationIndex;
    bool searchFound;
    float stateTimer;
    float resultTimer;
    int operationValue;
    int pendingInsertValue;
    bool isRunAtOnce;

    

    AVLTreeVisualizer();
    void handleInput();
    void updateAnimation(float deltaTime);
    void draw();
    void drawButton(Rectangle rect, const char* text, Color color);
    void drawTree(AVLNode* node, float x, float y, const std::set<AVLNode*>& highlight);
    void DrawCodeBox();
    void animateInsert(int value);
    void animateDelete(int value);
    void animateSearch(int value);
    void animateRandom();
    void animateClear();
    void animateLoadFile();
    void animatePrevious();
    void animateNext();
};

