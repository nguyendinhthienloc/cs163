#ifndef UI_MENU_H
#define UI_MENU_H

#include "raylib.h"
#include <string>

// Enum for managing different screens
typedef enum {
    MENU,
    LINKED_LIST
} Screen;

extern Screen currentScreen;
extern float transitionAlpha;
extern bool transitioning;
extern Screen nextScreen;
// UI Functions for Menu
void DrawMenu();
void DrawBackButton();
bool DrawButton(Rectangle rect, const char* text, Color color, bool enabled = true);
void TransitionTo(Screen newScreen);

#endif // UI_MENU_H