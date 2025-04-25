#include "../../header/Hash Table/HashTable.h"

int state = -1;

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

void HashTable::InsertInstantly(int val) {
    ResetColors(); // Clear previous highlights
    int index = hashFunction(val);
    Node* temp = table[index];

    // Check if value already exists
    while (temp) {
        if (temp->val == val) {
            foundNode = temp; // Highlight the existing node
            searchMessage = TextFormat("%d already exists.", val);
            state = 4;
            return;
        }
        temp = temp->next;
    }

    float startX = GetScreenWidth() / 8.0f;
    float startY = GetScreenHeight() / 10.0f;
    float boxWidth = GetScreenWidth() / 12.0f;
    float boxHeight = GetScreenHeight() / 20.0f;

    // Insert the node directly without animation/search
    Node* newNode = new Node{ val, table[index] };
    newNode->position = {
        startX + index * boxWidth,
        startY + (float)(GetNodeCount(index) + 1) * (boxHeight + GetScreenHeight() / 100)
    };

    table[index] = newNode;

    // Highlight the inserted node
    insertedNode = newNode;
    highlightInsert = index;
    highlightTimer = highlightDuration;

    searchMessage = TextFormat("Inserted %d successfully.", val);
    state = 6;
}

int HashTable::GetNodeCount(int index) {
    int count = 0;
    Node* temp = table[index];
    while (temp) {
        count++;
        temp = temp->next;
    }
    return count;
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

void HashTable::DeleteInstantly(int val) {
    ResetColors();
    undoStack.push(CopyTable());
    while (!redoStack.empty()) redoStack.pop();

    int index = hashFunction(val);
    Node* curr = table[index];
    Node* prev = nullptr;

    while (curr) {
        if (curr->val == val) {
            nodeToDelete = nullptr;  // No need to animate or deleteTimer
            deleteTimer = 0.0f;  // Skip animation

            // Perform the deletion immediately
            if (prev == nullptr) {
                table[index] = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            state = 4;
            searchMessage = TextFormat("Deleted %d successfully.", val);
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    nodeToDelete = nullptr;
    deleteTimer = 0.0f;
    searchMessage = TextFormat("Value %d not found!", val);
    state = 6;
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

void HashTable::StartInstantSearch(int val) {
    ResetColors();
    searchValue = val;
    visitedNode = table[hashFunction(val)];
    highlightSearch = hashFunction(val);  // Highlight the starting position

    // Check if the value is found instantly
    if (visitedNode) {
        while (visitedNode) {
            if (visitedNode->val == searchValue) {
                foundNode = visitedNode;
                searchMessage = TextFormat("Value %d found!", searchValue);
                highlightSearch = hashFunction(val);  // Highlight the found node
                state = 4;
                break;
            }
            visitedNode = visitedNode->next;
        }
    }

    // If no node is found, set the message to indicate the value is not found
    if (!visitedNode) {
        searchMessage = TextFormat("Value %d not found!", searchValue);
        state = 6;
        highlightSearch = -1;  // No node to highlight if not found
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
    state = 0;
    checkWhileCondition = 0;
}

void HashTable::UpdateValueAnimation(int oldVal, int newVal) {
    ResetColors();
    undoStack.push(CopyTable());
    while (!redoStack.empty()) redoStack.pop();

    int index = hashFunction(oldVal);
    if (hashFunction(oldVal) != hashFunction(newVal)) {
        searchMessage = TextFormat("%d and %d have different hash values!", oldVal, newVal);
        state = 6;
        return;
    }

    // Start first search: check if new value exists
    searching = true;
    searchValue = newVal; // Check new value first
    pendingValue = oldVal; // Store old value for second search
    newValueForUpdate = newVal;
    visitedNode = table[index];
    highlightSearch = index;
    searchTimer = GetTime();
    searchMessage = TextFormat("Checking if %d exists: %d %% %d = %d", newVal, newVal, HT_SIZE, index);
    state = 0;
    checkWhileCondition = 0;
    pendingOp = PendingOperation::CHECK_NEW_VALUE; // New operation for first search
}

void HashTable::UpdateValueInstantly(int oldVal, int newVal) {
    ResetColors();
    undoStack.push(CopyTable());
    while (!redoStack.empty()) redoStack.pop();

    int index = hashFunction(oldVal);
    if (hashFunction(oldVal) != hashFunction(newVal)) {
        searchMessage = TextFormat("%d and %d have different hash values!", oldVal, newVal);
        state = -1;
        return;
    }

    pendingOp = PendingOperation::CHECK_NEW_VALUE;
    // Check for new value first
    Node* curr = table[index];
    while (curr) {
        if (curr->val == newVal) {
            foundNode = curr;
            searchMessage = TextFormat("New value %d already exists!", newVal);
            highlightSearch = index;
            state = 4;
            return;
        }
        curr = curr->next;
    }

    // New value not found, check for old value
    pendingOp = PendingOperation::UPDATE;
    curr = table[index];
    while (curr) {
        if (curr->val == oldVal) {
            curr->val = newVal;
            foundNode = curr;
            searchMessage = TextFormat("Updated %d to %d successfully.", oldVal, newVal);
            highlightSearch = index;
            highlightTimer = highlightDuration; // Highlight updated node
            state = 4;
            return;
        }
        curr = curr->next;
    }

    // Old value not found
    searchMessage = TextFormat("Value %d not found!", oldVal);
    state = 6;
}

void HashTable::UpdateSearchAnimation() {
    if (searching && GetTime() - searchTimer >= 0.5) {
        if (checkWhileCondition == 0) {
            state = 1; // Highlight "while (HT[i] && HT[i]->val != key)"
            if (!visitedNode) {
                state = 6; // Not found
                searchMessage = TextFormat("Value %d not found!", searchValue);
                searching = false;
                highlightSearch = -1;
                checkWhileCondition = 0;
            }
            else if (visitedNode->val == searchValue) {
                foundNode = visitedNode;
                state = 4; // Found
                searchMessage = TextFormat("Value %d found!", searchValue);
                visitedNode = nullptr;
                searching = false;
                highlightSearch = -1;
                checkWhileCondition = 0;
            }
            else {
                checkWhileCondition = 1; // Next cycle: advance pointer
            }
        }
        else if (checkWhileCondition == 1) {
            state = 2; // Highlight "HT[i] = HT[i]->next"
            visitedNode = visitedNode->next;
            checkWhileCondition = 2; // Next cycle: if check
        }
        else if (checkWhileCondition == 2) {
            if (visitedNode) {
                checkWhileCondition = 0; // Next cycle: while condition
            }
            else {
                state = 6; // Not found
                searchMessage = TextFormat("Value %d not found!", searchValue);
                searching = false;
                highlightSearch = -1;
                checkWhileCondition = 0;
            }
        }

        searchTimer = GetTime();
        if (!searching) {
            if (pendingOp == PendingOperation::CHECK_NEW_VALUE) {
                if (state == 4) {
                    // New value found, stop
                    searchMessage = TextFormat(" % d already exists!", searchValue);
                    //pendingOp = PendingOperation::NONE;
                    pendingValue = -1;
                    newValueForUpdate = -1;
                }
                else {
                    // New value not found, start second search for old value
                    int oldVal = pendingValue;
                    int index = hashFunction(oldVal);
                    searching = true;
                    searchValue = oldVal;
                    pendingValue = searchValue; // Will be updated to new value later
                    visitedNode = table[index];
                    highlightSearch = index;
                    searchTimer = GetTime();
                    searchMessage = TextFormat("Checking if %d exists: %d %% %d = %d", oldVal, oldVal, HT_SIZE, index);
                    state = 0;
                    checkWhileCondition = 0;
                    pendingOp = PendingOperation::UPDATE;
                }
            }
            else if (pendingOp == PendingOperation::UPDATE) {
                if (state == 4) {
                    // Old value found, update to new value
                    foundNode->val = newValueForUpdate; // Update will be handled below
                    searchMessage = TextFormat("Updated %d to %d successfully.", pendingValue, newValueForUpdate);
                    highlightTimer = highlightDuration;
                    //pendingOp = PendingOperation::NONE;
                    pendingValue = -1;
                    newValueForUpdate = -1;
                }
                else {
                    // Old value not found
                    searchMessage = TextFormat("Value %d not found!", searchValue);
                    pendingOp = PendingOperation::NONE;
                    pendingValue = -1;
                    newValueForUpdate = -1;
                }
            }
            else if (pendingOp == PendingOperation::INSERT) {
                if (!WasValueFound()) {
                    state = 6;
                    PerformInsertion(pendingValue, false);
                    insertedNode = table[hashFunction(pendingValue)];
                    highlightInsert = hashFunction(pendingValue);
                    highlightTimer = highlightDuration;
                    searchMessage = TextFormat("Inserted %d successfully.", pendingValue);
                }
                else {
                    state = 4;
                    searchMessage = TextFormat("%d already exists.", pendingValue);
                }
            }
            else if (pendingOp == PendingOperation::DELETE) {
                if (WasValueFound()) {
                    state = 4;
                    Delete(pendingValue);
                }
                else {
                    state = 6;
                    searchMessage = TextFormat("%d does not exist!", pendingValue);
                }
            }
            if (pendingOp != PendingOperation::UPDATE && pendingOp != PendingOperation::CHECK_NEW_VALUE) {
                //pendingOp = PendingOperation::NONE;
                pendingValue = -1;
            }
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
        DrawText(TextFormat("%d", i), startX + i * boxWidth + boxWidth / 3+10, indexY, boxHeight / 2, BLACK);
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

        Vector2 adjustedPos = { curr->position.x, curr->position.y};

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
            Vector2 nextAdjustedPos = curr->next->position;
            Vector2 start = { adjustedPos.x + nodeBox.width / 2, adjustedPos.y + nodeBox.height };
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
        //DrawText(calculationText.c_str(), messageX, messageY, screenHeight / 25, GREEN);
        DrawTextEx(codeFont, calculationText.c_str(), { messageX, messageY }, screenHeight / 25, 1, BLACK);

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
        //DrawText(searchMessage.c_str(), messageX, messageY, screenHeight / 25, BLACK);
        DrawTextEx(codeFont, searchMessage.c_str(), { messageX, messageY }, screenHeight / 25, 1, BLACK);
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
    float maxNodeHeight = boxHeight;
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

    static Vector2 prevMouse = mouse;  // Keep track of previous frame's mouse pos

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mouse, { 20, 20, 100, 40 })) {
        isDragging = true;
        prevMouse = mouse;
    }

    if (isDragging) {
        Vector2 delta = { mouse.x - prevMouse.x, mouse.y - prevMouse.y };
        float newOffsetX = tableOffset.x + delta.x;
        float newOffsetY = tableOffset.y + delta.y;

        float minX = -((screenWidth - tableWidth) / 2);
        float maxX = screenWidth - tableWidth - minX;
        float minY = 0;
        float maxY = screenHeight - tableHeight;

        tableOffset.x = (newOffsetX < minX) ? minX : (newOffsetX > maxX) ? maxX : newOffsetX;
        tableOffset.y = (newOffsetY < minY) ? minY : (newOffsetY > maxY) ? maxY : newOffsetY;

        prevMouse = mouse; // Update for the next frame
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