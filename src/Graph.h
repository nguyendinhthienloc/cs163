#pragma once

#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <fstream>
#include <iostream>

#define WIDTH 1600
#define HEIGHT 900
//#define NUM_NODES 10
#define NODE_RADIUS 15
#define SPRING_LENGTH 300
#define SPRING_STRENGTH 0.01f
#define REPULSION_STRENGTH 5000
#define DAMPING 0.95f
#define MARGIN 80 
#define CENTERING_FORCE 0.0005f
#define TOP_MARGIN 100
#define BOTTOM_MARGIN 150
#define MIN_DISTANCE 30.0f 
#define LEFT_MARGIN 100
#define RIGHT_MARGIN 100

extern int stateOfCode;
extern std::string message;
extern int animationSpeed;

struct Node {
    Vector2 position;
    Vector2 velocity = { 0, 0 };
    bool isConsidered = false;

    //Node(Vector2 pos) : position(pos) {}
};

struct Edge {
    int a, b;  // Indices of nodes in the nodes array
    int w;
    bool inMST = false;
};

class Graph {
private:
    int V=0;
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    struct DSU {
        std::vector<int> parent;
        std::vector<int> size;

        void Initialize(int n);
        int find_set(int u);
        bool union_sets(int u, int v);
    } dsu;

    size_t kruskalStep = 0;
    int frameCounter = 0;
    bool mstFinished = false;
public:

    enum State {
        NORMAL, MST
    };

    State state = NORMAL;

    void ApplySpringForces();
    void RandomGraph();
    void Draw() const;
    void DrawGraph();
    void clearGraph();
    void setVertexCount(int vertexCount);
    void addEdge(int u, int v, int w);
    void addNode(int node);
    void resetGraph();

    // Kruskal's methods
    void StartKruskalAnimation();
    void UpdateKruskalStep();
    bool isMSTFinished() const { return mstFinished; }

    void loadFromFile(std::wifstream& fin);
};

