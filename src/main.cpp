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
        DrawEverything();
    }

    CloseWindow();
    return 0;
}
