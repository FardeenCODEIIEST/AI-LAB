#include <bits/stdc++.h>
using namespace std;

struct Node
{
    string id;                             // Label
    int value;                             // Heuristic Value
    bool isGoal;                           // whether goal state or not
    bool isObstacle;                       // whether obstacle or not
    double totalCost;                      // totalCost -> actual + heuristic
    vector<Node *> successors;             // successor nodes in the path
    vector<pair<Node *, Node *>> andPairs; // Nodes joined via hyperedge

    Node(string id, int value, bool isGoal = false, bool isObstacle = false)
        : id(id), value(value), isGoal(isGoal), isObstacle(isObstacle), totalCost(numeric_limits<double>::infinity()) {}
};

class AOStarSearch
{
private:
    Node *startNode;
    unordered_map<string, Node *> graph; // Label to Node map

    void evaluateNode(Node *node, set<string> &visited)
    {
        if (visited.count(node->id))
            return;
        visited.insert(node->id);

        if (node->isGoal)
        {
            node->totalCost = node->value;
            return;
        }

        // First evaluate all successors
        for (auto &successor : node->successors)
        {
            if (!successor->isObstacle && !visited.count(successor->id))
            {
                evaluateNode(successor, visited);
            }
        }

        // OR nodes
        double minCost = numeric_limits<double>::infinity();
        for (auto &successor : node->successors)
        {
            if (!successor->isObstacle)
            {
                double cost = node->value + successor->totalCost + 1; // 1 for the actual edge cost
                minCost = min(minCost, cost);
            }
        }

        // AND pairs
        for (const auto &andPair : node->andPairs)
        {
            if (!andPair.first->isObstacle && !andPair.second->isObstacle)
            {
                double andCost = node->value + andPair.first->totalCost + andPair.second->totalCost + 2; // taking twice edge cost
                minCost = min(minCost, andCost);
            }
        }

        node->totalCost = minCost;
    }

public:
    AOStarSearch(Node *start) : startNode(start)
    {
        graph[start->id] = start;
    }

    void addNode(Node *node)
    {
        graph[node->id] = node;
    }

    void addOREdge(const string &u, const string &v)
    {
        if (graph.count(u) && graph.count(v))
        {
            graph[u]->successors.push_back(graph[v]);
        }
    }

    void addANDPair(const string &u, const string &v1, const string &v2)
    {
        if (graph.count(u) && graph.count(v1) && graph.count(v2))
        {
            graph[u]->andPairs.push_back({graph[v1], graph[v2]});
        }
    }

    vector<string> findPath()
    {
        vector<string> path;
        set<string> visited;

        // Initial evaluation
        evaluateNode(startNode, visited);
        visited.clear();

        // Trace the path
        Node *current = startNode;
        while (current && !visited.count(current->id))
        {
            path.push_back(current->id);
            visited.insert(current->id);

            if (current->isGoal)
                break;

            // Find next node
            Node *next = NULL;
            double minCost = numeric_limits<double>::infinity();

            // Check regular successors
            for (auto &successor : current->successors)
            {
                if (!successor->isObstacle && !visited.count(successor->id))
                {
                    if (successor->totalCost + 1 < minCost)
                    {
                        minCost = successor->totalCost + 1;
                        next = successor;
                    }
                }
            }

            // Check AND pairs
            for (const auto &andPair : current->andPairs)
            {
                if (!andPair.first->isObstacle && !andPair.second->isObstacle)
                {
                    double pairCost = andPair.first->totalCost + andPair.second->totalCost + 2;
                    if (pairCost < minCost)
                    {
                        minCost = pairCost;
                        // Choose the unvisited node with lower cost
                        if (!visited.count(andPair.first->id) &&
                            (!visited.count(andPair.second->id) && andPair.first->totalCost <= andPair.second->totalCost))
                        {
                            next = andPair.first;
                        }
                        else if (!visited.count(andPair.second->id))
                        {
                            next = andPair.second;
                        }
                    }
                }
            }

            current = next;
        }

        return path;
    }

    void printCosts()
    {
        cout << "Final Node costs:\n";
        for (const auto &[id, node] : graph)
        {
            cout << id << ": " << node->totalCost << "\n";
        }
    }

    ~AOStarSearch()
    {
        for (auto &[id, node] : graph)
        {
            delete node;
        }
    }
};

int main()
{
    Node *nodeA = new Node("A", 0);
    Node *nodeB = new Node("B", 4);
    Node *nodeC = new Node("C", 0, false, true);
    Node *nodeD = new Node("D", 10);
    Node *nodeE = new Node("E", 3);
    Node *nodeF = new Node("F", 0, false, true);
    Node *nodeG = new Node("G", 0, false, true);
    Node *nodeH = new Node("H", 2);
    Node *nodeI = new Node("I", 8);
    Node *nodeJ = new Node("J", 0, false, true);
    Node *nodeK = new Node("K", 3);
    Node *nodeL = new Node("L", 5, true);

    AOStarSearch search(nodeA);

    vector<Node *> nodes = {nodeA, nodeB, nodeC, nodeD, nodeE, nodeF,
                            nodeG, nodeH, nodeI, nodeJ, nodeK, nodeL};
    for (const auto &node : nodes)
    {
        search.addNode(node);
    }

    // OR edges
    search.addOREdge("A", "B");
    search.addOREdge("A", "D");
    search.addOREdge("A", "E");
    search.addOREdge("B", "H");
    search.addOREdge("B", "K");
    search.addOREdge("E", "H");
    search.addOREdge("E", "K");
    search.addOREdge("H", "I");
    search.addOREdge("K", "L");
    search.addOREdge("I", "L");

    // AND pairs    ---> HyperEdges
    search.addANDPair("A", "B", "E");
    search.addANDPair("B", "H", "K");
    search.addANDPair("E", "H", "K");

    vector<string> path = search.findPath();

    cout << "Optimal path: ";
    for (int i = 0; i < path.size(); ++i)
    {
        cout << path[i];
        if (i < path.size() - 1)
        {
            cout << " -> ";
        }
    }
    cout << "\n";

    search.printCosts();

    return 0;
}
