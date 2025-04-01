#pragma once
#include "raylib.h"

class Graph;

extern Graph G;
extern Font codeFont;

bool DrawButton(Rectangle rect, const char* text, Color color, int shift, Font font, float FontSize);
void DefineButtons(Rectangle& randomBtn, Rectangle& LoadFileBtn, Rectangle& mstBtn, Rectangle& openMenuBtn, Rectangle& clearBtn);
void DrawAndHandleButtons();
void DrawOutLine();
bool LoadFileGraph();
void DefineCodeTableElements(Rectangle& openBtn, Rectangle& startAlgo1, Rectangle& startAlgo2, Rectangle& forLoop, Rectangle& condition, Rectangle& add, Rectangle& skip, Rectangle& done);
void DrawCodeTable();
void DrawChosenAlgo();
void DrawCurrentStep();
void DefineSpeedSlider(float& minX, float& maxX, float& y, float& minValue, float& maxValue);
void DrawSpeedSlider();
void UpdateSlider();