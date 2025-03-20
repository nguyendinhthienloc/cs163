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

	ht.searchMessage = "";
}

int HashTable::hashFunction(int val) {
	return val % HT_SIZE;
}

void HashTable::Insert(int val, bool isRandom) {
	ResetHighlight();

	int index = hashFunction(val);
	Node* temp = table[index];

	while (temp) {
		if (temp->val == val) {
			if (!isRandom) {
				searchMessage = TextFormat("%d already exists.", val);
			}
			return; // Duplicate found, no state change
		}
		temp = temp->next;
	}

	if (!isRandom) { // Save state only if not random
		undoStack.push(CopyTable());
		while (!redoStack.empty()) redoStack.pop();
		showCalculation = true;
		insertedNode = nullptr;
	}
	else {
		showCalculation = false;
		insertedNode = nullptr;
		highlightInsert = -1;
	}

	PerformInsertion(val, isRandom);
}

void HashTable::PerformInsertion(int val, bool isRandom) {
	int index = hashFunction(val);

	Node* newNode = new Node{ val, table[index] };
	newNode->position = { (float)GetScreenWidth(), (float)GetScreenHeight() };
	table[index] = newNode;

	if (!isRandom) {  // Only highlight if it's a manual insertion
		insertedNode = newNode;
		highlightInsert = index;
		highlightTimer = highlightDuration;
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
			nodeToDelete = curr; // Mark for deletion
			deleteTimer = deleteDuration; // Start animation
			highlightInsert = index; // Highlight bucket
			if (prev == nullptr) {
				table[index] = curr->next;
			}
			else {
				prev->next = curr->next;
			}
			return; // Exit after setting up deletion
		}
		prev = curr;
		curr = curr->next;
	}
	// If no node is found, reset state to avoid confusion
	nodeToDelete = nullptr;
	deleteTimer = 0.0f;
	searchMessage = TextFormat("Value %d not found!", val); // Immediate feedback
}

void HashTable::UpdateDeletionAnimation() {
	if (nodeToDelete && deleteTimer > 0.0f) {
		deleteTimer -= GetFrameTime();
		if (deleteTimer <= 0.0f) {
			delete nodeToDelete; // Free memory
			nodeToDelete = nullptr; // Reset to nullptr after deletion
			highlightInsert = -1; // Reset highlight
			searchMessage = "Deleted successfully."; // Set success message
			deleteTimer = 0.0f; // Ensure timer is reset
		}
	}
	else if (!nodeToDelete && deleteTimer > 0.0f) {
		// This should not happen, but reset if it does
		deleteTimer = 0.0f;
		searchMessage = "Deletion failed."; // Optional fallback message
	}
}

void HashTable::StartSearch(int val, bool isInsert) {
	ResetHighlight();

	searching = true;
	searchValue = val;
	visitedNode = table[hashFunction(val)];
	highlightSearch = hashFunction(val);
	searchTimer = GetTime();

	searchMessage = TextFormat("%d %% %d = %d", searchValue, HT_SIZE, highlightSearch);
}

void HashTable::UpdateSearchAnimation() {
	if (searching && GetTime() - searchTimer >= 0.5) {
		if (!visitedNode) {
			searchMessage = TextFormat("Value %d not found!", searchValue);
			searching = false;
			highlightSearch = -1;
		}
		else if (visitedNode->val == searchValue) {
			foundNode = visitedNode;
			if (pendingOp != PendingOperation::DELETE) {
				// Only show "found" message for Search operation, not Delete
				searchMessage = TextFormat("Value %d found!", searchValue);
			}
			visitedNode = nullptr;
			searching = false;
			highlightSearch = -1;
		}
		else {
			visitedNode = visitedNode->next;
			if (!visitedNode) {
				searchMessage = TextFormat("Value %d not found!", searchValue);
				searching = false;
				highlightSearch = -1;
			}
		}

		searchTimer = GetTime();

		if (!searching) {
			if (pendingOp == PendingOperation::INSERT) {
				if (!WasValueFound()) {
					PerformInsertion(pendingValue, false);
					insertedNode = table[hashFunction(pendingValue)];
					highlightInsert = hashFunction(pendingValue);
					highlightTimer = highlightDuration;
					searchMessage = TextFormat("Inserted %d successfully.", pendingValue);
				}
				else {
					searchMessage = TextFormat("%d already exists.", pendingValue);
				}
			}
			else if (pendingOp == PendingOperation::DELETE) {
				if (WasValueFound()) {
					Delete(pendingValue); // Trigger deletion, no message here
				}
				else {
					searchMessage = TextFormat("%d does not exist!", pendingValue);
				}
			}
			pendingOp = PendingOperation::NONE;
			pendingValue = -1;
		}
	}
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

	DrawBucketIndexes(startX, indexY, boxWidth, boxHeight);
	DrawBuckets(startX, startY, boxWidth, boxHeight);
	DrawCalculationText(screenWidth, screenHeight);
	DrawSearchMessage(screenWidth, screenHeight);
}

void HashTable::DrawBucketIndexes(float startX, float indexY, float boxWidth, float boxHeight) {
	for (int i = 0; i < HT_SIZE; i++) {
		DrawText(TextFormat("%d", i), startX + i * boxWidth + boxWidth / 3, indexY, boxHeight / 2, BLACK);
	}
}

void HashTable::DrawBuckets(float startX, float startY, float boxWidth, float boxHeight) {
	for (int i = 0; i < HT_SIZE; i++) {
		Color bucketColor = LIGHTGRAY;
		if (i == highlightInsert) bucketColor = GREEN;
		if (i == highlightSearch) bucketColor = BLUE;
		Rectangle bucket = { startX + i * boxWidth, startY, boxWidth - 2, boxHeight };
		DrawRectangleRec(bucket, bucketColor);
		DrawRectangleLines(bucket.x, bucket.y, bucket.width, bucket.height, DARKGRAY);
		DrawNodes(i, startX, startY, boxWidth, boxHeight);
	}
}

void HashTable::DrawNodes(int index, float startX, float startY, float boxWidth, float boxHeight) {
	Node* curr = table[index];
	int offset = 1;
	while (curr) {
		// Calculate the target position for the node (relative to the bucket)
		Vector2 targetPos = { startX + index * boxWidth, startY + offset * (boxHeight + GetScreenHeight() / 100) };
		// Update the node's position with lerp (this position is not yet offset)
		curr->position.x += (targetPos.x - curr->position.x) * 0.1f;
		curr->position.y += (targetPos.y - curr->position.y) * 0.1f;

		// Apply tableOffset to the node's position for drawing
		Vector2 adjustedPos = { curr->position.x + tableOffset.x, curr->position.y + tableOffset.y };

		// Set the color based on the node's state
		Color color = RAYWHITE;
		Color borderColor = DARKGRAY;
		if (curr == insertedNode) color = GREEN;
		if (curr == visitedNode) color = YELLOW;
		if (curr == foundNode) color = BLUE;
		if (curr == nodeToDelete) color = Fade(RED, deleteTimer / deleteDuration);

		// Draw the node at the adjusted position
		Rectangle nodeBox = { adjustedPos.x, adjustedPos.y, boxWidth - 2, boxHeight };
		DrawRectangleRec(nodeBox, color);
		DrawRectangleLines(nodeBox.x, nodeBox.y, nodeBox.width, nodeBox.height, borderColor);

		// Draw the text inside the node
		int textWidth = MeasureText(TextFormat("%d", curr->val), boxHeight / 2);
		int textX = nodeBox.x + (nodeBox.width - textWidth) / 2;
		int textY = nodeBox.y + (nodeBox.height - boxHeight / 2) / 2;
		DrawText(TextFormat("%d", curr->val), textX, textY, boxHeight / 2, BLACK);

		// Draw arrow to the next node if it exists
		if (curr->next) {
			// Adjust the next node's position with tableOffset
			Vector2 nextAdjustedPos = { curr->next->position.x + tableOffset.x, curr->next->position.y + tableOffset.y };
			// Draw the line from the current node to the next node
			Vector2 start = { nodeBox.x + nodeBox.width / 2, nodeBox.y + nodeBox.height };
			Vector2 end = { nextAdjustedPos.x + nodeBox.width / 2, nextAdjustedPos.y };
			DrawLineEx(start, end, 2.0f, BLACK);

			// Draw the arrowhead
			Vector2 dir = { end.x - start.x, end.y - start.y };
			float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
			if (len > 0) {
				dir = { dir.x / len, dir.y / len };
				Vector2 perp = { -dir.y, dir.x };
				Vector2 tip = { end.x - dir.x * 10, end.y - dir.y * 10 };
				DrawTriangle(
					end,
					{ tip.x + perp.x * 5, tip.y + perp.y * 5 },
					{ tip.x - perp.x * 5, tip.y - perp.y * 5 },
					BLACK
				);
			}
		}

		curr = curr->next;
		offset++;
	}
}

void HashTable::DrawCalculationText(float screenWidth, float screenHeight) {
	if (highlightTimer > 0.0f && showCalculation && insertedNode) {
		std::string calculationText = TextFormat("%d %% %d = %d", insertedNode->val, HT_SIZE, highlightInsert);
		float textWidth = MeasureText(calculationText.c_str(), screenHeight / 25);
		float messageX = (screenWidth - textWidth) / 2;
		float messageY = screenHeight - screenHeight / 10 - 30; // Position above searchMessage
		DrawText(calculationText.c_str(), messageX, messageY, screenHeight / 25, GREEN);

		highlightTimer -= GetFrameTime();
		if (highlightTimer <= 0.0f) {
			insertedNode = nullptr;
			highlightInsert = -1;
			showCalculation = false;
		}
	}
}

void HashTable::DrawSearchMessage(float screenWidth, float screenHeight) {
	if (!searchMessage.empty()) {
		float messageX = screenWidth / 2 - MeasureText(searchMessage.c_str(), screenHeight / 25) / 2;
		float messageY = screenHeight - screenHeight / 10; // Keep at bottom
		DrawText(searchMessage.c_str(), messageX, messageY, screenHeight / 25, BLACK);
	}
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
	srand(time(NULL));

	Clear();  // Clear the table first
	while (!redoStack.empty()) redoStack.pop();  // Clear redo stack

	int elementsToInsert = maxElements > HT_SIZE ? HT_SIZE : maxElements;
	for (int i = 0; i < elementsToInsert; i++) {
		int randomValue = minValue + (rand() % (maxValue - minValue + 1));
		Insert(randomValue, true);
	}

	undoStack.push(CopyTable());  // Save the state after random insertions
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
		ResetColors();                // Reset all highlights and animations
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
	searching = false;
	nodeToDelete = nullptr; // Add this
	deleteTimer = 0.0f;    // Add this
}

void HashTable::ResetColors() {
	insertedNode = nullptr;
	visitedNode = nullptr;
	foundNode = nullptr;
	nodeToDelete = nullptr; // Ensure this is reset
	highlightInsert = -1;
	highlightSearch = -1;
	highlightTimer = 0.0f;
	deleteTimer = 0.0f;    // Reset deletion timer
	showCalculation = false;
	searching = false;
	searchMessage = "";
}

