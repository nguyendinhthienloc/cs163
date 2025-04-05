#pragma once
#include "raylib.h"
#include "InputBox.h"

class Graph;

extern Graph G;
extern Font codeFont;

bool DrawButton(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize);
void DefineButtons(Rectangle& randomBtn, Rectangle& LoadFileBtn, Rectangle& mstBtn, Rectangle& openMenuBtn, Rectangle& clearBtn, Rectangle& inputBtn, Rectangle& loadMatrix, Rectangle& loadEdge);
void DrawAndHandleButtons();
void DefineTextBoxElements(Rectangle& menu);
bool DrawTextBox();
void DrawOutLine();
bool LoadFileGraph(int choice);
void DefineCodeTableElements(Rectangle& openBtn, Rectangle& startAlgo1, Rectangle& startAlgo2, Rectangle& forLoop, Rectangle& condition, Rectangle& add, Rectangle& skip, Rectangle& done);
void DrawCodeTable();
void DrawChosenAlgo();
void DrawCurrentStep();
void DefineSpeedSlider(float& minX, float& maxX, float& y, float& minValue, float& maxValue);
void DrawSpeedSlider();
void UpdateSlider();
void DefinePauseButton(Rectangle& pauseBtn);
void DrawPauseButton();