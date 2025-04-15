#ifndef UI_LINKED_LIST_H
#define UI_LINKED_LIST_H

#include "raylib.h"
#include "linked_list.h"
#include <string>

struct InputState {
    char inputBuffer[5];
    int inputLength;
    bool inputActive;
};

void DrawLinkedList(LinkedList& list, InputState& inputState, std::string& feedbackMessage, double& feedbackTimer, bool& isDragging, float& dragStartX);
bool ValidateInput(const char* input, int& value);

#endif // UI_LINKED_LIST_H