#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

struct Node {
    int value;
    Vector2 position;
    Vector2 targetPosition;
    Node* next;
    float alpha;
    bool selected;

    Node(int val, Vector2 pos) : value(val), position(pos), targetPosition(pos), next(nullptr), alpha(1.0f), selected(false) {}
};

struct Operation {
    enum class Type { INSERT, DELETE, INSERT_AFTER, INSERT_HEAD, INSERT_TAIL };
    Type type;
    int value;
    Vector2 position;
    Node* prevNode;
    Node* targetNode;

    Operation(Type t, int v, Vector2 pos = {0, 0}, Node* prev = nullptr, Node* target = nullptr) 
        : type(t), value(v), position(pos), prevNode(prev), targetNode(target) {}
};

class LinkedList {
private:
    Vector2 curPos;
    Node* head;
    Node* foundNode;
    Node* cur;
    Node* selectedNode;
    std::vector<Node*> nodes;
    float nodeSpacing = 100;
    float scrollOffsetX = 0.0f;
    float scrollOffsetY = 0.0f;
    bool searchAttempted = false;
    bool stepMode = false;
    bool stepPaused = false;

    enum class AnimState { IDLE, INSERTING, DELETING, SEARCHING, INSERTING_AFTER, SEARCHING_FOR_DELETE, RELINKING, GENERATING, MOVING };
    AnimState animState = AnimState::IDLE;
    float animProgress = 0.0f;
    Node* animNode = nullptr;
    Vector2 animStartPos;
    float highlightTimer = 0.0f;
    Node* animPrevNode = nullptr;
    Node* animNextNode = nullptr;
    std::string currentPseudoCode;

    std::vector<Operation> history;

    bool isUndoing = false;
    Operation::Type undoOperationType;

    void RecalculatePositions();
    Vector2 CalculateArrowStart(Vector2 from, Vector2 to, float radius);
    Vector2 CalculateArrowEnd(Vector2 from, Vector2 to, float radius);
    void SetPseudoCode(const std::string& code);

public:
    LinkedList() : head(nullptr), foundNode(nullptr), cur(nullptr), selectedNode(nullptr), scrollOffsetX(0), scrollOffsetY(0), animStartPos{0, 0}, curPos{0, 0}, stepMode(false), stepPaused(false), currentPseudoCode(""), isUndoing(false), undoOperationType(Operation::Type::INSERT) {}
    ~LinkedList();

    int GetSize();
    void Insert(int value);
    void InsertHead(int value);
    void InsertTail(int value);
    void InsertAfter(Node* target, int value);
    void Delete(int value);
    bool Search(int value);
    void Update(int oldValue, int newValue);
    Node* GetNodeAt(int index);
    void Undo();
    bool LoadFromFile(const char* filename);

    void UpdateAnimation(float deltaTime);
    void Draw();

    float GetScrollOffsetX() const { return scrollOffsetX; }
    void SetScrollOffsetX(float offset) { scrollOffsetX = offset; }
    float GetScrollOffsetY() const { return scrollOffsetY; }
    void SetScrollOffsetY(float offset) { scrollOffsetY = offset; }

    bool IsAnimating() const { return animState != AnimState::IDLE; }
    Node* GetFoundNode() const { return foundNode; }
    void SetFoundNode(Node* node) { foundNode = node; }
    Node* GetSelectedNode() const { return selectedNode; }
    void SetSelectedNode(Node* node) { selectedNode = node; }

    const std::vector<Operation>& GetHistory() const { return history; }
    void ClearHistory();
    const std::string& GetPseudoCode() const { return currentPseudoCode; }

    bool IsStepMode() const { return stepMode; }
    void ToggleStepMode() { stepMode = !stepMode; stepPaused = stepMode; }
    void NextStep() { if (stepMode) stepPaused = false; }

    Node* GetHead();
    void SetHead(Node* newHead);
    std::vector<Node*>& GetNodes();
    void ResetAnimationState();
};
#endif // LINKED_LIST_H