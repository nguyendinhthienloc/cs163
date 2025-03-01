#include "raylib.h"
#include "LinkedList.h"
#include <string>

// Prevent window resizing in fullscreen mode
void UpdateWindowSize(LinkedList& list) {
    if (IsWindowFullscreen()) return; // Prevent resizing in fullscreen

    int nodeCount = list.getSize();
    int minWidth = 800;
    int extraSpace = nodeCount * 120;
    int newWidth = minWidth + extraSpace;

    SetWindowSize(newWidth, GetMonitorHeight(0)); // Resize only in windowed mode
}

int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Singly Linked List Visualizer");
    ToggleFullscreen();  // Start in fullscreen mode
    SetTargetFPS(60);

    LinkedList list;
    std::string userInput = "";
    bool typingMode = false;
    bool deletingMode = false;

    while (!WindowShouldClose()) {
        if (typingMode) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= '0' && key <= '9') userInput += (char)key;
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !userInput.empty()) userInput.pop_back();
            if (IsKeyPressed(KEY_ENTER) && !userInput.empty()) {
                int value = std::stoi(userInput);
                list.insert(value);
                UpdateWindowSize(list);
                userInput = "";
                typingMode = false;
            }
        } else if (deletingMode) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= '0' && key <= '9') userInput += (char)key;
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !userInput.empty()) userInput.pop_back();
            if (IsKeyPressed(KEY_ENTER) && !userInput.empty()) {
                int value = std::stoi(userInput);
                list.remove(value);
                UpdateWindowSize(list);
                userInput = "";
                deletingMode = false;
            }
        } else {
            if (IsKeyPressed(KEY_I)) { typingMode = true; userInput = ""; }
            if (IsKeyPressed(KEY_D)) { deletingMode = true; userInput = ""; }
            if (IsKeyPressed(KEY_F)) {
                if (IsWindowFullscreen()) {
                    ToggleFullscreen();
                    SetWindowSize(800, 600); // Restore to default size after exiting fullscreen
                } else {
                    ToggleFullscreen();
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Press 'I' to insert a node", 10, 10, 20, DARKGRAY);
        DrawText("Press 'D' to delete a node", 10, 40, 20, DARKGRAY);
        DrawText("Press 'F' to toggle fullscreen", 10, 70, 20, DARKGRAY);
        if (typingMode) DrawText(("Inserting: " + userInput).c_str(), 10, 100, 20, BLUE);
        if (deletingMode) DrawText(("Deleting: " + userInput).c_str(), 10, 140, 20, RED);
        list.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
