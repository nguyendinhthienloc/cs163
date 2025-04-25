#pragma once
#include "raylib.h"  // Using raylib.h directly
#include "../Font.h"
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <ctime>

enum class PendingOperation { NONE, INSERT, DELETE, SEARCH };
extern int state;

class HashTable {
public:
    HashTable();
    ~HashTable();

    void Draw();
    std::string searchMessage = "";

    bool IsSearching() const { return searching; }
    bool WasValueFound() const { return foundNode != nullptr; }
    void ResetColors();

    PendingOperation GetPendingOperation() const { return pendingOp; }
    void SetPendingOperation(PendingOperation op, int val) {
        pendingOp = op;
        pendingValue = val;
    }
    int GetPendingValue() const { return pendingValue; }

    int GetNodeCount(int index);
    void InsertInstantly(int val);
    void Insert(int val, bool isRandom);
    void PerformInsertion(int val, bool isRandom);
    void DeleteInstantly(int val);
    void Delete(int val);
    void UpdateDeletionAnimation();
    void StartInstantSearch(int val);
    void StartSearch(int val, bool isInsert);
    void UpdateSearchAnimation();
    void RandomInsert(int maxElements, int minValue, int maxValue);
    void Clear();
    void HandleTableDragging();  // Keep this for bounded dragging
    void Undo();
    void Redo();
    void cleanUp();

private:
    static const int HT_SIZE = 13;

    PendingOperation pendingOp = PendingOperation::NONE;
    int pendingValue = -1;

    bool isDragging = false;
    Vector2 dragStart = { 0, 0 };
    Vector2 tableOffset = { 0, 0 };

    struct Node {
        int val;
        Node* next;
        Vector2 position;
    };

    std::vector<Node*> table;
    std::stack<std::vector<Node*>> undoStack;
    std::stack<std::vector<Node*>> redoStack;

    int hashFunction(int val);
    std::vector<Node*> CopyTable();
    void FreeTable(std::vector<Node*>& tableCopy);

    void DrawBucketIndexes(float startX, float indexY, float boxWidth, float boxHeight);
    void DrawBuckets(float startX, float startY, float boxWidth, float boxHeight);
    void DrawNodes(int index, float startX, float startY, float boxWidth, float boxHeight);
    void DrawCalculationText(float screenWidth, float screenHeight);
    void DrawSearchMessage(float screenWidth, float screenHeight);

    Node* insertedNode = nullptr;
    int highlightInsert = -1;
    float highlightTimer = 0.0f;
    const float highlightDuration = 2.0f;
    bool showCalculation = false;

    Node* nodeToDelete = nullptr;
    float deleteTimer = 0.0f;
    const float deleteDuration = 1.0f;

    int highlightSearch = -1;
    Node* visitedNode = nullptr;
    bool searching = false;
    int searchValue = -1;
    float searchTimer = 0.0f;
    Node* foundNode = nullptr;
    int checkWhileCondition = 0;
};