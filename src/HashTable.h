#pragma once
#include "hashtable_ui.h"
#include "vector"
#include <string>
#include <stack> // For undo/redo
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <functional>
#include <thread>
#include <chrono>

class HashTable {
public:
	HashTable();
	~HashTable();

	void Draw();
	std::string searchMessage = "";

	// New methods to check search state
	bool IsSearching() const { return searching; }
	bool WasValueFound() const { return foundNode != nullptr; }
	void ResetColors(); // New method to reset all colors

	// New state for pending operations
	enum class PendingOperation { NONE, INSERT, DELETE };
	PendingOperation GetPendingOperation() const { return pendingOp; }
	void SetPendingOperation(PendingOperation op, int val) {
		pendingOp = op;
		pendingValue = val;
	}
	int GetPendingValue() const { return pendingValue; }

	void Insert(int val, bool isRandom);
	void PerformInsertion(int val, bool isRandom);
	void Delete(int val);
	void UpdateDeletionAnimation();
	void StartSearch(int val, bool isInsert);
	void UpdateSearchAnimation();
	void RandomInsert(int maxElements, int minValue, int maxValue);
	void Clear();
	void HandleTableDragging();

	void Undo();
	void Redo();

	void ResetHighlight();

	void cleanUp();

	void ResetTableOffset() { tableOffset = { 0, 0 }; }

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
		Vector2 position;  // Store position for animation
	};

	std::vector<Node*> table;
	std::stack<std::vector<Node*>> undoStack;
	std::stack<std::vector<Node*>> redoStack;

	int hashFunction(int val);
	std::vector<Node*> CopyTable();
	void FreeTable(std::vector<Node*>& tableCopy);

	// Drawing function
	void DrawBucketIndexes(float startX, float indexY, float boxWidth, float boxHeight);   // Draws the hash table buckets
	void DrawBuckets(float startX, float startY, float boxWidth, float boxHeight);
	void DrawNodes(int index, float startX, float startY, float boxWidth, float boxHeight);     // Draws the nodes inside the buckets
	void DrawCalculationText(float screenWidth, float screenHeight); // Displays the hash calculation message
	void DrawSearchMessage(float screenWidth, float screenHeight);   // Displays the search result message

	// Variables for inserting
	Node* insertedNode = nullptr;  // Track the last inserted node
	int highlightInsert = -1;     // Track the red bucket index
	float highlightTimer = 0.0f;   // Timer for how long to keep it red
	const float highlightDuration = 2.0f; // Duration in seconds
	bool showCalculation = false;

	// Variables for deleting
	Node* nodeToDelete = nullptr; // Track node to delete
	float deleteTimer = 0.0f;    // Timer for deletion animation
	const float deleteDuration = 1.0f; // Duration of fade-out

	// Variable for searching
	int highlightSearch = -1;
	Node* visitedNode = nullptr;  // Stores the most recent visited node
	bool searching = false;
	int searchValue = -1;
	float searchTimer = 0.0f;
	Node* foundNode = nullptr; // Stores the pointer to the found node
};