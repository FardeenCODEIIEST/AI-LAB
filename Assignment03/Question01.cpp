#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

struct Coord
{
    int x, y, z;
    bool operator==(const Coord &other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

string makeKey(const Coord &coord, int mask)
{
    ostringstream oss;
    oss << coord.x << "," << coord.y << "," << coord.z << "," << mask;
    return oss.str();
}

struct Node
{
    Coord coord;        // current coordinate (x, y, z)
    int mask;           // bitmask indicating which rescue goals have been visited
    double g, h, f;     // g: cost so far, h: heuristic, f: total cost
    vector<Coord> path; // full 3D path (list of coordinates)

    bool operator>(const Node &other) const
    {
        return f > other.f;
    }
};

class AStarDrone
{
private:
    vector<vector<vector<char>>> grid;
    vector<Coord> rescueGoals;
    Coord base;
    int nGoals;

    int dx[6] = {-1, 1, 0, 0, 0, 0};
    int dy[6] = {0, 0, -1, 1, 0, 0};
    int dz[6] = {0, 0, 0, 0, -1, 1};

    double euclidean(const Coord &a, const Coord &b)
    {
        return sqrt(pow(a.x - b.x, 2) +
                    pow(a.y - b.y, 2) +
                    pow(a.z - b.z, 2));
    }

    // Multi-goal heuristic:
    // If not all rescue goals are visited, h = distance(current, nearest unvisited goal) + distance(nearest goal, base)
    // Otherwise, h = distance(current, base)
    double heuristic(const Coord &current, int mask)
    {
        if (mask == (1 << nGoals) - 1)
            return euclidean(current, base);
        double best = numeric_limits<double>::max();
        double bestToBase = 0.0;
        for (int i = 0; i < nGoals; i++)
        {
            if (!(mask & (1 << i)))
            {
                double d = euclidean(current, rescueGoals[i]);
                if (d < best)
                {
                    best = d;
                    bestToBase = euclidean(rescueGoals[i], base);
                }
            }
        }
        return best + bestToBase;
    }

    // For free space ('0') cost is 1, fire ('F') cost is 3.
    // Additionally, moving upward (dz = 1) adds +2, moving downward (dz = -1) adds +1.
    int getCost(char terrain, int dz_move)
    {
        int baseCost = (terrain == 'F' ? 3 : 1);
        if (dz_move == 1)
            return baseCost + 2;
        else if (dz_move == -1)
            return baseCost + 1;
        else
            return baseCost;
    }

public:
    AStarDrone(vector<vector<vector<char>>> cityGrid, vector<Coord> rescueGoals, Coord base)
        : grid(cityGrid), rescueGoals(rescueGoals), base(base)
    {
        nGoals = rescueGoals.size();
    }

    // Returns a pair: (path, totalEnergyCost). An empty path indicates failure.
    pair<vector<Coord>, double> findPath(Coord start)
    {
        priority_queue<Node, vector<Node>, greater<Node>> pq;
        unordered_map<string, double> visited; // state key -> best f cost

        // Start state: no rescue goals visited.
        Node startNode{start, 0, 0.0, heuristic(start, 0), heuristic(start, 0), {start}};
        pq.push(startNode);

        while (!pq.empty())
        {
            Node current = pq.top();
            pq.pop();

            string key = makeKey(current.coord, current.mask);
            if (visited.find(key) != visited.end() && visited[key] <= current.f)
                continue;
            visited[key] = current.f;

            for (int i = 0; i < nGoals; i++)
            {
                if (!(current.mask & (1 << i)) && current.coord == rescueGoals[i])
                {
                    current.mask |= (1 << i);
                }
            }
            if (current.mask == (1 << nGoals) - 1 && current.coord == base)
                return {current.path, current.g};

            for (int i = 0; i < 6; i++)
            {
                Coord next{current.coord.x + dx[i],
                           current.coord.y + dy[i],
                           current.coord.z + dz[i]};

                if (next.x < 0 || next.y < 0 || next.z < 0 ||
                    next.x >= grid.size() ||
                    next.y >= grid[0].size() ||
                    next.z >= grid[0][0].size())
                    continue;
                if (grid[next.x][next.y][next.z] == '1')
                    continue;

                int moveCost = getCost(grid[next.x][next.y][next.z], dz[i]);
                double gNew = current.g + moveCost;
                double hNew = heuristic(next, current.mask);
                double fNew = gNew + hNew;

                vector<Coord> newPath = current.path;
                newPath.push_back(next);

                Node neighbor{next, current.mask, gNew, hNew, fNew, newPath};
                string nKey = makeKey(neighbor.coord, neighbor.mask);
                if (visited.find(nKey) == visited.end() || visited[nKey] > neighbor.f)
                {
                    pq.push(neighbor);
                }
            }
        }
        return {{}, 0.0};
    }
};

string cellDescription(char cell)
{
    switch (cell)
    {
    case 'S':
        return "Start";
    case '0':
        return "Free space";
    case 'F':
        return "Fire zone";
    case 'G':
        return "Rescue";
    case 'R':
        return "Recharge";
    case 'B':
        return "Base";
    default:
        return "";
    }
}

int moveCost(const vector<vector<vector<char>>> &grid, const Coord &next, int dz)
{
    char cell = grid[next.x][next.y][next.z];
    int baseCost = (cell == 'F' ? 3 : 1);
    if (dz == 1)
        return baseCost + 2;
    else if (dz == -1)
        return baseCost + 1;
    else
        return baseCost;
}

int main()
{
    int layers, n, m;
    cout << "Enter the number of layers:\n";
    cin >> layers;
    cout << "Enter the width:\n";
    cin >> m;
    cout << "Enter the length:\n";
    cin >> n;
    vector<vector<vector<char>>> grid(n, vector<vector<char>>(m, vector<char>(layers)));
    vector<Coord> rescueGoals;
    Coord start = {-1, -1, -1};
    Coord base = {-1, -1, -1};
    for (int i = 0; i < layers; i++)
    {
        cout << "Layer " << i << ":\n";
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < m; k++)
            {
                cout << "Cell <" << j << ", " << k << "> : ";
                cin >> grid[j][k][i];
                if (grid[j][k][i] == 'B')
                    base = {j, k, i};
                else if (grid[j][k][i] == 'S')
                    start = {j, k, i};
                else if (grid[j][k][i] == 'G')
                    rescueGoals.push_back({j, k, i});
            }
        }
    }
    cout << "The Grid is:\n";
    for (int i = 0; i < layers; i++)
    {
        cout << "Layers " << i << ": \n";
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < m; k++)
            {
                cout << grid[j][k][i] << " ";
            }
            cout << "\n";
        }
    }
    if (start.x == -1)
    {
        cout << "No path found." << "\n";
        return 0;
    }
    AStarDrone drone(grid, rescueGoals, base);

    auto result = drone.findPath(start);
    vector<Coord> path = result.first;
    double totalEnergy = result.second;

    if (path.empty())
    {
        cout << "No path found." << "\n";
    }
    else
    {
        cout << "Optimal Path Using A*:" << "\n"
             << "\n";
        double energySum = 0.0;
        for (size_t i = 0; i < path.size(); i++)
        {
            if (i == 0)
            {
                cout << "Step " << i + 1 << ": Start at (" << path[i].x << ", "
                     << path[i].y << ", " << path[i].z << ") -> "
                     << cellDescription(grid[path[i].x][path[i].y][path[i].z])
                     << " (Energy used: 0)" << "\n";
            }
            else
            {
                int dx = path[i].x - path[i - 1].x;
                int dy = path[i].y - path[i - 1].y;
                int dz = path[i].z - path[i - 1].z;
                int cost = moveCost(grid, path[i], dz);
                energySum += cost;
                string moveDesc = "";
                if (dz == 1)
                    moveDesc = "Climb to ";
                else if (dz == -1)
                    moveDesc = "Descend to ";
                else
                    moveDesc = "Move to ";

                string cellDesc = cellDescription(grid[path[i].x][path[i].y][path[i].z]);
                cout << "Step " << i + 1 << ": " << moveDesc << "(" << path[i].x << ", "
                     << path[i].y << ", " << path[i].z << ") -> "
                     << cellDesc << " (Energy used: " << cost << ")" << "\n";
            }
        }
        cout << "\n";
        cout << "Total Energy Used: " << totalEnergy << " units" << "\n";
        cout << "Rescue Status: ";
        for (size_t i = 0; i < rescueGoals.size(); i++)
        {
            cout << "G" << i + 1 << " ";
            if (i < rescueGoals.size() - 1)
                cout << ", ";
        }
        cout << "\n";
        cout << "Mission Completion: Success! The drone returned to base ("
             << base.x << ", " << base.y << ", " << base.z << ")" << "\n";
    }

    return 0;
}
