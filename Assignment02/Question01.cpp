/*
    A* search algorithm employs searching the search space using both heuristic(h) and
    actual cost(g) combined into a objective function(f) that is used to penetrate the search
    space by going from the start state to the final state making decisions at the current
    state.
    The admissibility is ensured.
    The heuristic function h(n) is referred to as acceptable,
    and it is always less than or equal to the actual cost,
    which is the true cost from the present point in the path.
    H(n) is never bigger than h*(n) in this context.

*/

#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int x, y;                // Current coordinates
    double g_cost = DBL_MAX; // Cost from start to current node
    double h_cost = 0;       // Estimated cost from current to goal
    double f_cost = DBL_MAX; // Total cost (g + h)
    Node *parent = nullptr;  // Parent node

    Node(int x, int y) : x(x), y(y) {}

    bool operator<(const Node &other) const
    {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }
};

class Compare
{
public:
    bool operator()(const Node *below, const Node *above)
    {
        return below->f_cost > above->f_cost;
    };
};

class GridSolver
{
private:
    int width, height;
    vector<vector<int>> grid;

    vector<pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    double calculateHCostEuclidean(const Node &current, const Node &goal)
    {
        double dx = current.x - goal.x;
        double dy = current.y - goal.y;
        return sqrt(dx * dx + dy * dy);
    }

    double calculateHCostManhattan(const Node &current, const Node &goal)
    {
        double dx = abs(current.x - goal.x) * 1.0;
        double dy = abs(current.y - goal.y) * 1.0;
        return dx + dy;
    }

    bool isValid(int x, int y)
    {
        return x >= 0 && x < width && y >= 0 && y < height && grid[y][x] != 0;
    }

public:
    GridSolver(int w, int h, const vector<vector<int>> &obstacles)
        : width(w), height(h), grid(obstacles) {}

    pair<vector<pair<int, int>>, double> findPath(
        pair<int, int> start,
        pair<int, int> goal, bool useManhattan)
    {
        // Check if start and end are valid
        if (!isValid(start.first, start.second) || !isValid(goal.first, goal.second))
        {
            return {vector<pair<int, int>>(), DBL_MAX};
        }

        Node startNode(start.first, start.second);
        Node goalNode(goal.first, goal.second);

        priority_queue<Node *, vector<Node *>, Compare> pq;
        map<Node, Node *> nodes;

        startNode.g_cost = 0;
        startNode.h_cost = useManhattan ? calculateHCostManhattan(startNode, goalNode) : calculateHCostEuclidean(startNode, goalNode);
        startNode.f_cost = startNode.g_cost + startNode.h_cost;

        Node *current = new Node(startNode);
        nodes[startNode] = current;
        pq.push(current);

        while (!pq.empty())
        {
            current = pq.top();
            pq.pop();

            if (current->x == goalNode.x && current->y == goalNode.y)
            {
                vector<pair<int, int>> path;
                double finalCost = current->g_cost;

                while (current != NULL)
                {
                    path.push_back({current->x, current->y});
                    current = current->parent;
                }
                reverse(path.begin(), path.end());

                for (auto &pair : nodes)
                {
                    delete pair.second;
                }

                return {path, finalCost};
            }

            for (const auto &dir : directions)
            {
                int newX = current->x + dir.first;
                int newY = current->y + dir.second;

                if (!isValid(newX, newY))
                    continue;

                // double movementCost = (dir.first != 0 && dir.second != 0) ? sqrt(2.0) : 1.0;
                double movementCost = 1.0; // Considering diagonal movement is treated same as vertical or horizontal movement
                double tentative_g = current->g_cost + movementCost;

                Node neighbor(newX, newY);
                if (nodes.find(neighbor) == nodes.end())
                {
                    nodes[neighbor] = new Node(newX, newY);
                }

                Node *neighborNode = nodes[neighbor];
                if (tentative_g < neighborNode->g_cost)
                {
                    neighborNode->parent = current;
                    neighborNode->g_cost = tentative_g;
                    neighborNode->h_cost = useManhattan ? calculateHCostManhattan(*neighborNode, goalNode) : calculateHCostEuclidean(*neighborNode, goalNode);
                    neighborNode->f_cost = neighborNode->g_cost + neighborNode->h_cost;
                    pq.push(neighborNode);
                }
            }
        }

        for (auto &pair : nodes)
        {
            delete pair.second;
        }
        return {vector<pair<int, int>>(), DBL_MAX};
    }
};

int main()
{
    int m, n;
    cout << "Enter the number of rows of the grid:\n";
    cin >> m;
    cout << "Enter the number of columns in the grid:\n";
    cin >> n;

    vector<vector<int>> grid(m, vector<int>(n, 1));

    cout << "Enter the number of obstacles in the grid:\n";
    int q;
    cin >> q;
    while (q--)
    {
        cout << "Enter the obstacle coordinates:\n";
        int x, y;
        cin >> x >> y;
        if (x >= 0 && x < m && y >= 0 && y < n)
        {
            grid[x][y] = 0;
        }
    }

    GridSolver gs(n, m, grid);

    cout << "Enter the start state coordinates:\n";
    int sx, sy;
    cin >> sx >> sy;
    cout << "Enter the goal state coordinates:\n";
    int ex, ey;
    cin >> ex >> ey;

    // Manhattan heuristic
    auto result = gs.findPath({sy, sx}, {ey, ex}, true);
    if (result.second == DBL_MAX)
    {
        cout << "Could not find a path using Manhattan heuristic\n";
    }
    else
    {
        cout << "Path found using Manhattan heuristic with cost " << result.second << ":\n";
        for (int i = 0; i < result.first.size(); i++)
        {
            if (i < result.first.size() - 1)
                cout << "< " << result.first[i].second << " , " << result.first[i].first << " > ---- ";
            else
                cout << "< " << result.first[i].second << " , " << result.first[i].first << " >";
        }
        cout << "\n";
    }

    // Euclidean heuristic
    result = gs.findPath({sy, sx}, {ey, ex}, false);
    if (result.second == DBL_MAX)
    {
        cout << "Could not find a path using Euclidean heuristic\n";
    }
    else
    {
        cout << "Path found using Euclidean heuristic with cost " << result.second << ":\n";
        for (int i = 0; i < result.first.size(); i++)
        {
            if (i < result.first.size() - 1)
                cout << "< " << result.first[i].second << " , " << result.first[i].first << " > ---- ";
            else
                cout << "< " << result.first[i].second << " , " << result.first[i].first << " >";
        }
        cout << "\n";
    }

    return 0;
}
