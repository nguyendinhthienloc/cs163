#include "linked_list.h"
#include "ui.h"
#include "raylib.h"

// UI for linked list operations
void DrawLinkedListUI(LinkedList &list) {
    DrawText("Singly Linked List Visualization", 500, 50, 24, DARKGRAY);

    if (DrawButton({600, 100, 180, 50}, "Insert", GREEN)) {
        list.Insert(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 160, 180, 50}, "Delete", RED)) {
        list.Delete(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 220, 180, 50}, "Search", BLUE)) {
        list.Search(GetRandomValue(1, 99));
    }
    if (DrawButton({600, 280, 180, 50}, "Update", ORANGE)) {
        list.Update(GetRandomValue(1, 99), GetRandomValue(1, 99));
    }

    list.Draw();
}
