#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "linked_list.h"
#include <string>

// Enum for managing different screens
typedef enum {
    MENU,
    LINKED_LIST
} Screen;

extern Screen currentScreen;

// Struct to track input state
struct InputState {
    char inputBuffer[5];   // Buffer for user input
    int inputLength;       // Length of input
    bool inputActive;      // Flag to track if the input box is active
};

// UI Functions

// Draw the entire UI
void DrawUI(LinkedList& list, bool& isFullScreen, InputState& inputState);

// Draw the main menu
void DrawMenu();

// Draw the LinkedList visualization
void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX);

// Draw the Back button in the UI
void DrawBackButton();

// Draw a button and check for its click event
bool DrawButton(Rectangle rect, const char* text, Color color, bool enabled = true);

// Transition between screens
void TransitionTo(Screen newScreen);

// Validate input for correct integer values within a specified range
bool ValidateInput(const char* input, int& value);

#endif // UI_H
