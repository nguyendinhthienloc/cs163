#include "raylib.h"
#include "ui_Graph.h"
#include "Graph.h"
#include "Graph.h"
#include <vector>
#include <cmath>

int main() {
    InitWindow(WIDTH, HEIGHT, "Minimum Spanning Tree  Visualization");
    SetTargetFPS(60);

    codeFont = LoadFont("Courier-Bold.ttf");

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawOutLine();

        DrawAndHandleButtons();
        DrawChosenAlgo();
        DrawCurrentStep();
        DrawSpeedSlider();
        UpdateSlider();

        G.DrawGraph();

        DrawCodeTable();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
