#include "Graph.h"
#include "raylib.h"

int stateOfCode = -1;
std::string message = "";
int animationSpeed = 90;

Graph::Graph() {
    RandomGraph();
}

void Graph::setVertexCount(int vertexCount) {
    V = vertexCount;
}

void Graph::addEdge(int u, int v, int w) {
    for (auto edge : edges) {
        if ((edge.a == u && edge.b == v) || (edge.a == v && edge.b == u)) {
            return;
        }
    }
    if (u < V && v < V && u >= 0 && v >= 0) { // Bounds check
        edges.push_back({ u, v, w });
    }
}

void Graph::addNode(int node) {
    Node newNode = { {
            static_cast<float>(GetRandomValue(MARGIN, WIDTH - MARGIN)),
            static_cast<float>(GetRandomValue(MARGIN, HEIGHT - MARGIN))
        }, {0, 0}, false };
    nodes.push_back(newNode);
}

// Function to apply forces using the spring algorithm
void Graph::ApplySpringForces() {
    Vector2 center = { WIDTH / 2.0f, HEIGHT / 2.0f };

    // Repulsion force (Coulomb's Law)
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = i + 1; j < nodes.size(); j++) {
            Vector2 diff = { nodes[i].position.x - nodes[j].position.x, nodes[i].position.y - nodes[j].position.y };
            float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);
            if (dist < MIN_DISTANCE) dist = MIN_DISTANCE;  // Avoid division by zero

            float force = REPULSION_STRENGTH / (dist * dist * 0.3f);
            Vector2 direction = { diff.x / dist, diff.y / dist };  // Normalize

            nodes[i].velocity.x += direction.x * force;
            nodes[i].velocity.y += direction.y * force;
            nodes[j].velocity.x -= direction.x * force;
            nodes[j].velocity.y -= direction.y * force;
        }
    }

    // Attraction force (Hooke’s Law)
    for (const auto& edge : edges) {
        Node& nodeA = nodes[edge.a];
        Node& nodeB = nodes[edge.b];

        Vector2 diff = { nodeB.position.x - nodeA.position.x, nodeB.position.y - nodeA.position.y };
        float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);
        float displacement = dist - SPRING_LENGTH;
        Vector2 direction = { diff.x / dist, diff.y / dist };  // Normalize

        float force = SPRING_STRENGTH * displacement;
        nodeA.velocity.x += direction.x * force;
        nodeA.velocity.y += direction.y * force;
        nodeB.velocity.x -= direction.x * force;
        nodeB.velocity.y -= direction.y * force;
    }

    for (auto& node : nodes) {
        Vector2 diff = { center.x - node.position.x, center.y - node.position.y };
        float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);
        if (dist > MIN_DISTANCE) {  // Avoid division by zero
            Vector2 direction = { diff.x / dist, diff.y / dist };  // Normalize
            float force = CENTERING_FORCE * dist;  // Stronger pull if farther from center
            node.velocity.x += direction.x * force;
            node.velocity.y += direction.y * force;
        }
    }

    // Update positions with damping and screen boundaries
    for (auto& node : nodes) {
        node.velocity.x *= DAMPING;
        node.velocity.y *= DAMPING;
        node.position.x += node.velocity.x;
        node.position.y += node.velocity.y;

        // Boundary constraints: Keep nodes within the screen
        if (node.position.x < NODE_RADIUS + LEFT_MARGIN) {
            node.position.x = NODE_RADIUS + LEFT_MARGIN;
            node.velocity.x *= -0.5f;  // Bounce effect
        }
        if (node.position.x > WIDTH - NODE_RADIUS - RIGHT_MARGIN) {
            node.position.x = WIDTH - NODE_RADIUS - RIGHT_MARGIN;
            node.velocity.x *= -0.5f;
        }
        if (node.position.y < NODE_RADIUS + TOP_MARGIN) {  // Prevent nodes from colliding with UI at the top
            node.position.y = NODE_RADIUS + TOP_MARGIN;
            node.velocity.y *= -0.5f;
        }
        if (node.position.y > HEIGHT - NODE_RADIUS - BOTTOM_MARGIN) {  // Prevent nodes from colliding with UI at the bottom
            node.position.y = HEIGHT - NODE_RADIUS - BOTTOM_MARGIN;
            node.velocity.y *= -0.5f;
        }
    }
}

void Graph::RandomGraph() {
    clearGraph();
    std::random_device rd;  // Seed with a hardware random device
    std::mt19937 gen(rd()); // Mersenne Twister 19937 generator
    std::uniform_int_distribution<int> dist(3, 15); // Range [0, 19]
    std::uniform_int_distribution<int> weightDist(1, 99);

    V = dist(gen);
    for (int i = 0; i < V; i++) {
        addNode(i); // Construct nodes with random positions
    }

    // Generate random edges (ensuring a connected graph)
    for (int i = 0; i < V - 1; i++) {
        addEdge(i, i + 1, weightDist(gen));
        if (i < V - 2) {
            int randomNode = GetRandomValue(i + 2, V - 1);
            addEdge(i, randomNode, weightDist(gen));
        }
    }

    state = NORMAL;
    kruskalStep = 0;
    frameCounter = 0;
}

void Graph::Draw() const {
    Color SMOKE = { 180, 180, 180, 120 };

    // Draw edges
    for (size_t i = 0; i < edges.size(); i++) {
        const Edge& edge = edges[i];
        Color edgeColor = BLACK;

        if (state == MST) {
            if ((i == kruskalStep && !mstFinished && stateOfCode >= 2 && stateOfCode <= 5) || edge.inMST) {
                edgeColor = ORANGE;
            }
            else if (i < kruskalStep && !edge.inMST) {
                edgeColor = SMOKE; // Edge was considered but not chosen
            }
        }

        if (edge.a < nodes.size() && edge.b < nodes.size()) {
            DrawLineEx(nodes[edge.a].position, nodes[edge.b].position, 3.0f, edgeColor);

            Vector2 midpoint = {
                (nodes[edge.a].position.x + nodes[edge.b].position.x) / 2.0f,
                (nodes[edge.a].position.y + nodes[edge.b].position.y) / 2.0f
            };

            Vector2 direction = { nodes[edge.b].position.x - nodes[edge.a].position.x, nodes[edge.b].position.y - nodes[edge.a].position.y };

            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
            if (length > 0) {
                direction.x /= length;  // Normalize
                direction.y /= length;
            }

            Vector2 perpendicular = { -direction.y, direction.x };

            float offset = 20.0f;

            Vector2 textPosition = {
                midpoint.x + perpendicular.x * offset,
                midpoint.y + perpendicular.y * offset
            };

            std::string wText = std::to_string(edge.w);
            DrawText(wText.c_str(), textPosition.x, textPosition.y, 16, edgeColor);
        }
    }

    // Draw nodes
    for (size_t i = 0; i < nodes.size(); i++) {
        bool isInCurrentEdge = false;
        bool hasBeenConsidered = false;

        if (state == MST) {

            if (state == MST) {
                if (kruskalStep < edges.size() && !mstFinished && stateOfCode >= 2 && stateOfCode <= 5) {
                    const Edge& currentEdge = edges[kruskalStep];
                    if (currentEdge.a == i || currentEdge.b == i) {
                        isInCurrentEdge = true;
                    }
                }

                if (nodes[i].isConsidered) {
                    hasBeenConsidered = true;
                }
            }
        }

        Color fillColor = WHITE;
        Color outlineColor = BLACK;
        Color numberColor = BLACK;

        if (state == MST) {
            if (isInCurrentEdge) {
                fillColor = ORANGE;
                numberColor = WHITE;
                outlineColor = ORANGE;
            }
            else if (hasBeenConsidered) {
                fillColor = WHITE;
                numberColor = ORANGE;
                outlineColor = ORANGE;
            }
        }

        DrawCircleV(nodes[i].position, NODE_RADIUS, fillColor);

        for (float thickness = 0; thickness < 2.0f; thickness += 0.5f) {
            DrawCircleLinesV(nodes[i].position, NODE_RADIUS + thickness, outlineColor);
        }

        std::string vertexNum = std::to_string(i);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), vertexNum.c_str(), 20, 1);
        Vector2 textPos = { nodes[i].position.x - textSize.x / 2, nodes[i].position.y - textSize.y / 2 };
        DrawText(vertexNum.c_str(), textPos.x, textPos.y + 2, 20, numberColor);
    }
}

void Graph::DrawGraph() {
    for (int i = 0; i < 1000; i++) {
        ApplySpringForces();
    }
    if (state == MST) {
        UpdateKruskalStep();
    }
    Draw();
}

void Graph::clearGraph() {
    V = 0;
    nodes.clear();
    edges.clear();
    adjMatrix.clear();
    kruskalStep = 0;
    frameCounter = 0;
    state = NORMAL;
    message = "";
}

void Graph::DSU::Initialize(int n) {
    parent.resize(n);
    size.resize(n);

    for (int i = 0; i < n; i++) {
        parent[i] = i;
        size[i] = 1;
    }
}

int Graph::DSU::find_set(int u) {
    if (u == parent[u]) return u;
    return parent[u] = find_set(parent[u]);
}

bool Graph::DSU::union_sets(int u, int v) {
    u = find_set(u);
    v = find_set(v);

    if (v == u) return false;

    if (size[u] < size[v]) std::swap(u, v);

    size[u] += size[v];
    parent[v] = u;
    return true;
}

bool initialDelayComplete = false;

void Graph::StartKruskalAnimation() {
    if (nodes.empty()) return;

    state = MST;
    kruskalStep = 0;
    frameCounter = 0;
    mstFinished = false;
    stateOfCode = 1;
    initialDelayComplete = false;

    for (auto& node : nodes) {
        node.isConsidered = false;
    }
    for (auto& edge : edges) {
        edge.inMST = false;
    }
    message = "Sorting Edges ...";
}

bool isPaused = false;

void Graph::UpdateKruskalStep() {
    if (isPaused) {
        return;
    }
    frameCounter++;
    if (stateOfCode == 1) {
        if (!initialDelayComplete) {
            if (frameCounter < 2 * animationSpeed / 3) { // 1.5-second delay for state 1
                return;
            }
            // Perform initialization and sorting after delay
            dsu.Initialize(nodes.size());
            std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
                return a.w < b.w;
                });
            initialDelayComplete = true;
            frameCounter = 0;
        }
        if (frameCounter < animationSpeed / 3) { // Brief pause before moving to loop
            return;
        }
        stateOfCode = 2; // Move to for loop
        frameCounter = 0;
    }

    if (kruskalStep >= edges.size()) {
        stateOfCode = 6;
        int cost = 0;
        for (auto edge : edges) {
            if (edge.inMST) cost += edge.w;
        }
        message = "Total weight = " + std::to_string(cost);
        if (frameCounter < 4 * animationSpeed / 3) { // 2-second delay
            frameCounter++;
            return;
        }
        mstFinished = true;
        return;
    }

    Edge& currentEdge = edges[kruskalStep];
    if (frameCounter < animationSpeed / 3) { // Brief delay before checking edge
        message = "Cheking edge (" + std::to_string(currentEdge.w) + ",(" + std::to_string(currentEdge.a) + "," + std::to_string(currentEdge.b) + "))";
        stateOfCode = 2; // for loop
        return;
    }
    if (frameCounter < 2 * animationSpeed / 3) { // Delay for condition check
        stateOfCode = 3; // if condition
        return;
    }

    if (currentEdge.a < nodes.size() && currentEdge.b < nodes.size()) {
        int rootA = dsu.find_set(currentEdge.a);
        int rootB = dsu.find_set(currentEdge.b);

        if (rootA != rootB) { // No cycle
            message = "Add edge (" + std::to_string(currentEdge.w) + ",(" + std::to_string(currentEdge.a) + "," + std::to_string(currentEdge.b) + "))";
            if (frameCounter < animationSpeed) {
                stateOfCode = 4; // Add to T
                return;
            }
            dsu.union_sets(rootA, rootB);
            currentEdge.inMST = true;
            nodes[currentEdge.a].isConsidered = nodes[currentEdge.b].isConsidered = true;
        }
        else {
            message = "Ignore edge (" + std::to_string(currentEdge.w) + ",(" + std::to_string(currentEdge.a) + "," + std::to_string(currentEdge.b) + "))";

            if (frameCounter < animationSpeed) {
                stateOfCode = 5; // Skip edge
                return;
            }
        }
    }

    frameCounter = 0;
    kruskalStep++;
}

void Graph::resetGraph() {
    for (auto& node : nodes) {
        node.isConsidered = false;
    }
    for (auto& edge : edges) {
        edge.inMST = false;
    }
    message = "";
}

void Graph::loadFromFile(std::wifstream& fin, int choice) {
    if (!fin) {
        std::cerr << "Error: Failed to open file!\n";
        return;
    }
    clearGraph();
    if (choice == 1) {
        int V;
        if (!(fin >> V) || V <= 0) {
            message = "Invalid input!";
            return;
        }
        adjMatrix.resize(V, std::vector<int>(V));
        setVertexCount(V);
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (!(fin >> adjMatrix[i][j])) {  // Check each read operation
                    clearGraph();
                    message = "Invalid input!";
                    return;
                }
            }
        }
        for (int i = 0; i < V; i++) {
            addNode(i);
        }
        for (int i = 0; i < V; i++) {
            for (int j = i + 1; j < V; j++) {
                if (adjMatrix[i][j] != adjMatrix[j][i]) {
                    clearGraph();
                    message = "Invalid input!";
                    return;
                }
                if (adjMatrix[i][j]) edges.push_back({ i, j, adjMatrix[i][j] });
            }
        }
    }
    else if (choice == 2) {
        int V, E;

        if (!(fin >> V >> E) || V <= 0 || E < 0) { // Single check for invalid input
            message = "Invalid input!";
            return;
        }

        setVertexCount(V);

        for (int i = 0; i < E; i++) {
            int u, v, w;
            if (!(fin >> u >> v >> w) || u >= V || v >= V) { // Single read operation check
                clearGraph();
                message = "Invalid input!";
                return;
            }
            addEdge(u, v, w);
        }

        adjMatrix.resize(V, std::vector<int>(V));

        for (auto edge : edges) {
            adjMatrix[edge.a][edge.b] = edge.w;
            adjMatrix[edge.b][edge.a] = edge.w;
        }

        for (int i = 0; i < V; i++) {
            addNode(i);
        }
    }
}

std::string Graph::edgeListToString() {
    std::string res = "";
    res += std::to_string(V) + " " + std::to_string(edges.size()) + "\n";
    for (auto edge : edges) {
        res += std::to_string(edge.a) + " " + std::to_string(edge.b) + " " + std::to_string(edge.w) + "\n";
    }
    return res;
}

bool Graph::stringToEdgeList(const std::string& str) {
    std::stringstream ss(str);
    int vCount, eCount;

    clearGraph();

    // Read V and E
    if (!(ss >> vCount >> eCount)) {
        message = "Invalid Edge List! (Invalid V and E counts)";
        return false;
    }

    if (vCount <= 0 || eCount < 0) {
        message = "Invalid Edge List! (V <= 0 or E < 0)";
        return false;
    }

    V = vCount;
    for (int i = 0; i < V; i++) {
        addNode(i);
    }

    std::string line;
    int a, b, w;
    int edgeCounter = 0;

    // Read edges from subsequent lines
    std::getline(ss, line); // Skip the rest of the first line (V E)

    while (std::getline(ss, line)) {
        std::stringstream edgeStream(line);
        if (!(edgeStream >> a >> b >> w)) {
            message = "Invalid Edge List! (Invalid edge format)";
            return false;
        }

        if (a < 0 || b < 0 || a >= V || b >= V) {
            message = "Invalid Edge List! (Edge vertices out of bounds)";
            return false;
        }

        edges.push_back({ a, b, w });
        edgeCounter++;
    }

    // Check if the number of edges matches the expected count
    if (edgeCounter != eCount) {
        message = "Invalid Edge List! (Edge count mismatch)";
        return false;
    }

    return true;
}

