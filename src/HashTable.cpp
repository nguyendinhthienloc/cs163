#include "../include/HashTable.h"

HashTable::HashTable() {
    table.resize(HT_SIZE);
    for (int i = 0; i < HT_SIZE; i++) table[i] = nullptr;
}

HashTable::~HashTable() {
    cleanUp();
}

void HashTable::cleanUp() {
    Clear();
    while (!undoStack.empty()) {
        std::vector<Node*> snapshot = undoStack.top();
        FreeTable(snapshot);
        undoStack.pop();
    }
    while (!redoStack.empty()) {
        std::vector<Node*> snapshot = redoStack.top();
        FreeTable(snapshot);
        redoStack.pop();
    }
    searchMessage = "";
    ResetColors();
}

int HashTable::hashFunction(int val) {
    return val % HT_SIZE;
}

void HashTable::Insert(int val, bool isRandom) {
    ResetColors();
    int index = hashFunction(val);
    Node* temp = table[index];
    while (temp) {
        if (temp->val == val) {
            if (!isRandom) searchMessage = TextFormat("%d already exists.", val);
            return;
        }
        temp = temp->next;
    }
    if (!isRandom) {
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
    if (!isRandom) {
        insertedNode = newNode;
        highlightInsert = index;
        highlightTimer = highlightDuration;
    }
}

void HashTable::Delete(int val) {
    ResetColors();
    undoStack.push(CopyTable());
    while (!redoStack.empty()) redoStack.pop();
    int index = hashFunction(val);
    Node* curr = table[index];
    Node* prev = nullptr;
    while (curr) {
        if (curr->val == val) {
            nodeToDelete = curr;
            deleteTimer = deleteDuration;
            highlightInsert = index;
            if (prev == nullptr) {
                table[index] = curr->next;
            }
            else {
                prev->next = curr->next;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    nodeToDelete = nullptr;
    deleteTimer = 0.0f;
    searchMessage = TextFormat("Value %d not found!", val);
}

void HashTable::UpdateDeletionAnimation() {
    if (nodeToDelete && deleteTimer > 0.0f) {
        deleteTimer -= GetFrameTime();
        if (deleteTimer <= 0.0f) {
            delete nodeToDelete;
            nodeToDelete = nullptr;
            highlightInsert = -1;
            searchMessage = "Deleted successfully.";
            deleteTimer = 0.0f;
        }
    }
    else if (!nodeToDelete && deleteTimer > 0.0f) {
        deleteTimer = 0.0f;
        searchMessage = "Deletion failed.";
    }
}

void HashTable::StartSearch(int val, bool isInsert) {
    ResetColors();
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
                    Delete(pendingValue);
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
        Vector2 targetPos = { startX + index * boxWidth, startY + offset * (boxHeight + GetScreenHeight() / 100) };
        curr->position.x += (targetPos.x - curr->position.x) * 0.1f;
        curr->position.y += (targetPos.y - curr->position.y) * 0.1f;

        Vector2 adjustedPos = { curr->position.x + tableOffset.x, curr->position.y + tableOffset.y };

        Color color = RAYWHITE;
        Color borderColor = DARKGRAY;
        if (curr == insertedNode) color = GREEN;
        if (curr == visitedNode) color = YELLOW;
        if (curr == foundNode) color = BLUE;
        if (curr == nodeToDelete) color = Fade(RED, deleteTimer / deleteDuration);

        Rectangle nodeBox = { adjustedPos.x, adjustedPos.y, boxWidth - 2, boxHeight };
        DrawRectangleRec(nodeBox, color);
        DrawRectangleLines(nodeBox.x, nodeBox.y, nodeBox.width, nodeBox.height, borderColor);

        int textWidth = MeasureText(TextFormat("%d", curr->val), boxHeight / 2);
        int textX = nodeBox.x + (nodeBox.width - textWidth) / 2;
        int textY = nodeBox.y + (nodeBox.height - boxHeight / 2) / 2;
        DrawText(TextFormat("%d", curr->val), textX, textY, boxHeight / 2, BLACK);

        if (curr->next) {
            Vector2 nextAdjustedPos = { curr->next->position.x + tableOffset.x, curr->next->position.y + tableOffset.y };
            Vector2 start = { nodeBox.x + nodeBox.width / 2, nodeBox.y + nodeBox.height };
            Vector2 end = { nextAdjustedPos.x + nodeBox.width / 2, nextAdjustedPos.y };
            DrawLineEx(start, end, 2.0f, BLACK);

            Vector2 dir = { end.x - start.x, end.y - start.y };
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len > 0) {
                dir = { dir.x / len, dir.y / len };
                Vector2 perp = { -dir.y, dir.x };
                Vector2 tip = { end.x - dir.x * 10, end.y - dir.y * 10 };
                DrawTriangle(end, { tip.x + perp.x * 5, tip.y + perp.y * 5 }, { tip.x - perp.x * 5, tip.y - perp.y * 5 }, BLACK);
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
        float messageY = screenHeight - screenHeight / 10 - 30;
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
        float messageY = screenHeight - screenHeight / 10;
        DrawText(searchMessage.c_str(), messageX, messageY, screenHeight / 25, BLACK);
    }
}

void HashTable::Clear() {
    undoStack.push(CopyTable());
    while (!redoStack.empty()) redoStack.pop();

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
    Clear();
    while (!redoStack.empty()) redoStack.pop();

    maxElements = 5 + rand() % (50 - 5 + 1);
    for (int i = 0; i < maxElements; i++) {
        int randomValue = minValue + (rand() % (maxValue - minValue + 1));
        Insert(randomValue, true);
    }
    undoStack.push(CopyTable());
}

void HashTable::HandleTableDragging() {
    Vector2 mouse = GetMousePosition();
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float boxWidth = (screenWidth * 0.95f) / HT_SIZE;
    float boxHeight = screenHeight / 20;
    float buttonHeight = screenHeight / 12;
    float padding = screenWidth / 50;

    // Calculate the total width and height of the table (including nodes)
    float tableWidth = HT_SIZE * boxWidth;
    float maxNodeHeight = boxHeight; // Minimum height (buckets only)
    for (int i = 0; i < HT_SIZE; i++) {
        Node* curr = table[i];
        int nodeCount = 0;
        while (curr) {
            nodeCount++;
            curr = curr->next;
        }
        float totalNodeHeight = nodeCount * (boxHeight + screenHeight / 100);
        if (totalNodeHeight > maxNodeHeight) maxNodeHeight = totalNodeHeight;
    }
    float tableHeight = buttonHeight + 2 * padding + boxHeight + maxNodeHeight;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isDragging = true;
        dragStart = mouse;
    }

    if (isDragging) {
        Vector2 delta = { mouse.x - dragStart.x, mouse.y - dragStart.y };
        float newOffsetX = tableOffset.x + delta.x;
        float newOffsetY = tableOffset.y + delta.y;

        // Clamp the offsets to keep the table within screen bounds
        float minX = -((screenWidth - tableWidth) / 2); // Leftmost boundary (centered initially)
        float maxX = screenWidth - tableWidth - minX;    // Rightmost boundary
        float minY = 0;                                  // Top boundary (allow some space above)
        float maxY = screenHeight - tableHeight;         // Bottom boundary

        tableOffset.x = (newOffsetX < minX) ? minX : (newOffsetX > maxX) ? maxX : newOffsetX;
        tableOffset.y = (newOffsetY < minY) ? minY : (newOffsetY > maxY) ? maxY : newOffsetY;

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
        redoStack.push(CopyTable());
        FreeTable(table);
        table = undoStack.top();
        undoStack.pop();
        ResetColors();
    }
}

void HashTable::Redo() {
    if (!redoStack.empty()) {
        undoStack.push(CopyTable());
        FreeTable(table);
        table = redoStack.top();
        redoStack.pop();
    }
}

void HashTable::ResetColors() {
    insertedNode = nullptr;
    visitedNode = nullptr;
    foundNode = nullptr;
    nodeToDelete = nullptr;
    highlightInsert = -1;
    highlightSearch = -1;
    highlightTimer = 0.0f;
    deleteTimer = 0.0f;
    showCalculation = false;
    searching = false;
    searchMessage = "";
}