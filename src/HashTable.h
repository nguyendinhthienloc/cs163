#pragma once
#include "ui.h"
#include "vector"
#include <string>
#include <stack> // For undo/redo
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <functional>

class HashTable {
public:
	HashTable();
	~HashTable();

	std::string searchMessage = "";

	void Insert(int val, bool isRandom);
	void Delete(int val);
	bool Search(int val);
	void Draw();
	void RandomInsert(int maxElements, int minValue, int maxValue);
	void Clear();
	void HandleTableDragging();

	void Undo();
	void Redo();

	void ResetHighlight();

	void cleanUp();

private:
	static const int HT_SIZE = 13;

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

	Node* foundNode = nullptr; // Stores the pointer to the found node
	bool isSearching = false;  // Flag to indicate if a search is in progress

	int hashFunction(int val);
	std::vector<Node*> CopyTable();
	void FreeTable(std::vector<Node*>& tableCopy);

	// Variables for inserting
	Node* insertedNode = nullptr;  // Track the last inserted node
	int highlightedIndex = -1;     // Track the red bucket index
	float highlightTimer = 0.0f;   // Timer for how long to keep it red
	const float highlightDuration = 2.0f; // Duration in seconds
	bool showCalculation = false;
};