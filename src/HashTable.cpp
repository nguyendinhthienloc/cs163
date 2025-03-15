#include "HashTable.h"

HashTable::HashTable() {
	table.resize(HT_SIZE);
	for (int i = 0; i < HT_SIZE; i++) table[i] = nullptr;
}

HashTable::~HashTable() {
	cleanUp();
}

void HashTable::cleanUp() {
	Clear(); // Clears the main hash table

	// Clear undoStack
	while (!undoStack.empty()) {
		std::vector<Node*> snapshot = undoStack.top();
		FreeTable(snapshot);  // Free all nodes in the snapshot
		undoStack.pop();
	}

	// Clear redoStack
	while (!redoStack.empty()) {
		std::vector<Node*> snapshot = redoStack.top();
		FreeTable(snapshot);  // Free all nodes in the snapshot
		redoStack.pop();
	}
}

int HashTable::hashFunction(int val) {
	return val % HT_SIZE;
}

void HashTable::Insert(int val, bool isRandom) {
	ResetHighlight();

	if (!isRandom) { // Not from random function
		undoStack.push(CopyTable());
		while (!redoStack.empty()) redoStack.pop();

		showCalculation = true;  // Show calculation for manual insertions
		insertedNode = nullptr;  // Reset inserted node when inserting manually
	}
	else {
		showCalculation = false;
		insertedNode = nullptr;  // Ensure no node is marked red when random inserting
		highlightedIndex = -1;   // Ensure no bucket is highlighted
	}

	int index = hashFunction(val);
	Node* temp = table[index];

	while (temp) {
		if (temp->val == val) return;
		temp = temp->next;
	}

	// Create new node and place it off-screen
	Node* newNode = new Node{ val, table[index] };
	newNode->position = { (float)GetScreenWidth(), (float)GetScreenHeight() };
	table[index] = newNode;

	if (!isRandom) {
		insertedNode = newNode;   // Only mark for manual insertions
		highlightedIndex = index;
		highlightTimer = highlightDuration;  // Start timer
	}
}

void HashTable::Delete(int val) {
	ResetHighlight();

	undoStack.push(CopyTable());
	while (!redoStack.empty()) redoStack.pop();

	int index = hashFunction(val);
	Node* curr = table[index];
	Node* prev = nullptr;

	while (curr) {
		if (curr->val == val) {
			// Animate deletion (fade out)
			for (float alpha = 1.0f; alpha > 0.0f; alpha -= 0.05f) {
				BeginDrawing();
				ClearBackground(RAYWHITE);
				DrawRectangle(curr->position.x, curr->position.y, 50, 30, Fade(RED, alpha));
				EndDrawing();
			}

			// Remove node after animation
			if (prev == nullptr) {
				table[index] = curr->next;
				delete curr;
			}
			else {
				prev->next = curr->next;
				delete curr;
			}
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

bool HashTable::Search(int val) {
	ResetHighlight(); // Reset previous highlight

	int index = hashFunction(val);
	Node* temp = table[index];

	while (temp) {
		if (temp->val == val) {
			foundNode = temp; // Store the found node for highlighting
			return true;
		}
		temp = temp->next;
	}

	return false;
}

void HashTable::Draw() {
	float screenWidth = GetScreenWidth();
	float screenHeight = GetScreenHeight();
	float buttonHeight = screenHeight / 12;
	float padding = screenWidth / 50;
	float boxWidth = (screenWidth * 0.95f) / HT_SIZE;
	float boxHeight = screenHeight / 20;

	float startX = (screenWidth - (HT_SIZE * boxWidth)) / 2 + tableOffset.x;
	float indexY = buttonHeight + 2 * padding + tableOffset.y;
	float startY = indexY + boxHeight / 2;

	for (int i = 0; i < HT_SIZE; i++) {
		Color bucketColor = (i == highlightedIndex) ? GREEN : LIGHTGRAY;
		DrawText(TextFormat("%d", i), startX + i * boxWidth + boxWidth / 3, indexY, boxHeight / 2, BLACK);

		Rectangle bucket = { startX + i * boxWidth, startY, boxWidth - 2, boxHeight };
		DrawRectangleRec(bucket, bucketColor);
		DrawRectangleLines(bucket.x, bucket.y, bucket.width, bucket.height, DARKGRAY);

		Node* curr = table[i];
		int offset = 1;
		while (curr) {
			Vector2 targetPos = { startX + i * boxWidth, startY + offset * (boxHeight + screenHeight / 100) };

			// Smoothly interpolate position
			curr->position.x += (targetPos.x - curr->position.x) * 0.1f;
			curr->position.y += (targetPos.y - curr->position.y) * 0.1f;

			// Node color logic
			Color color = (curr == insertedNode) ? GREEN : RAYWHITE;

			if (curr == foundNode) {
				color = BLUE; // Highlight found node in green
			}

			// Draw node with animation
			Rectangle nodeBox = { curr->position.x, curr->position.y, boxWidth - 2, boxHeight };
			DrawRectangleRec(nodeBox, color);
			DrawRectangleLines(nodeBox.x, nodeBox.y, nodeBox.width, nodeBox.height, DARKGRAY);

			// Calculate text size
			int textWidth = MeasureText(TextFormat("%d", curr->val), boxHeight / 2);
			int textX = nodeBox.x + (nodeBox.width - textWidth) / 2;
			int textY = nodeBox.y + (nodeBox.height - boxHeight / 2) / 2;

			// Draw centered text
			DrawText(TextFormat("%d", curr->val), textX, textY, boxHeight / 2, BLACK);

			curr = curr->next;
			offset++;
		}
	}

	// Show calculation text only if the insertion was not random
	if (highlightTimer > 0.0f) {
		if (showCalculation && insertedNode) {
			// Format the text message
			std::string calculationText = TextFormat("%d %% %d = %d", insertedNode->val, HT_SIZE, highlightedIndex);

			// Measure the text width for centering
			float textWidth = MeasureText(calculationText.c_str(), screenHeight / 25);
			float messageX = (screenWidth - textWidth) / 2;  // Center horizontally
			float messageY = screenHeight - screenHeight / 10; // Keep it near the bottom

			// Draw the text
			DrawText(calculationText.c_str(), messageX, messageY, screenHeight / 25, GREEN);
		}

		highlightTimer -= GetFrameTime();  // Decrease timer

		if (highlightTimer <= 0.0f) {
			insertedNode = nullptr;
			highlightedIndex = -1;
			showCalculation = false;
		}
	}

	float messageX = screenWidth / 2 - MeasureText(searchMessage.c_str(), screenHeight / 25) / 2;
	float messageY = screenHeight - screenHeight / 10;
	DrawText(searchMessage.c_str(), messageX, messageY, screenHeight / 25, BLUE);
}

void HashTable::Clear() {
	undoStack.push(CopyTable());  // Save the state before clearing
	while (!redoStack.empty()) redoStack.pop();  // Clear redo stack on new action

	for (int i = 0; i < HT_SIZE; i++) {
		Node* temp = table[i];
		Node* del = nullptr;
		while (temp) {
			del = temp;
			temp = temp->next;
			delete del;
		}
		table[i] = nullptr;
	}
}

void HashTable::RandomInsert(int maxElements, int minValue, int maxValue) {
	undoStack.push(CopyTable());  // Save the state before inserting multiple values
	while (!redoStack.empty()) redoStack.pop();  // Clear redo stack on new action

	for (int i = 0; i < HT_SIZE; i++) {
		Node* temp = table[i];
		Node* del = nullptr;
		while (temp) {
			del = temp;
			temp = temp->next;
			delete del;
		}
		table[i] = nullptr;
	}

	for (int i = 0; i < maxElements; i++) {
		int randomValue = minValue + (rand() % (maxValue - minValue + 1));
		Insert(randomValue, true);
	}
}

void HashTable::HandleTableDragging() {
	Vector2 mouse = GetMousePosition();

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		isDragging = true;
		dragStart = mouse;
	}

	if (isDragging) {
		Vector2 delta = { mouse.x - dragStart.x, mouse.y - dragStart.y };
		tableOffset.x += delta.x;
		tableOffset.y += delta.y;
		dragStart = mouse; // Update reference position
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		isDragging = false;
	}
}

std::vector<HashTable::Node*> HashTable::CopyTable() {
	std::vector<Node*> copy(HT_SIZE, nullptr);

	for (int i = 0; i < HT_SIZE; i++) {
		if (table[i]) {
			Node* src = table[i];
			Node* copyHead = new Node{ src->val, nullptr };
			Node* copyCurrent = copyHead;

			while (src->next) {
				src = src->next;
				copyCurrent->next = new Node{ src->val, nullptr };
				copyCurrent = copyCurrent->next;
			}

			copy[i] = copyHead;
		}
	}

	return copy;
}

void HashTable::FreeTable(std::vector<Node*>& tableCopy) {
	for (Node* head : tableCopy) {
		while (head) {
			Node* temp = head;
			head = head->next;
			delete temp;
		}
	}
}

void HashTable::Undo() {
	if (!undoStack.empty()) {
		redoStack.push(CopyTable());  // Save current state for redo
		FreeTable(table);             // Free current table
		table = undoStack.top();      // Restore old state
		undoStack.pop();
	}
}

void HashTable::Redo() {
	if (!redoStack.empty()) {
		undoStack.push(CopyTable());  // Save current state for undo
		FreeTable(table);             // Free current table
		table = redoStack.top();      // Restore redo state
		redoStack.pop();
	}
}

void HashTable::ResetHighlight() {
	foundNode = nullptr;
	isSearching = false;
}
