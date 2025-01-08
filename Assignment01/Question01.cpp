/*
    The given problem statement reduces to finding an element in the set of all
    possible hamiltonian circuits in a graph G(V,E) that has the least total cost.
    This is basically the travelling salesman problem (TSP).

    Assumptions:-
        1. The cost of going from city A to city B is same as that from city B to city A.
            So the distance matrix should be symmetric.

        2. The distance matrix is generated in a way that does not give stuck at local optima
            problem most of the time(alpha>0.95), so hill climbing algorithm can be used

    Heuristic , Stochastic Algorithm:-

    1. Find a random solution and calculate its optimizer mapping(total cost)
    2. Find the neightbours of the random solution that are 2 hamming distance away (1 swap) from each other.
    3. Compare the costs and swap if better neightbour is found.
    4. Peform steps 1 to 3 till max iterations or there exist no better neighbour
*/
#include <bits/stdc++.h>

using namespace std;

class TSP
{
public:
    vector<vector<int>> distances;
    int n;
    // Random instance generator for shuffling
    random_device rd;
    mt19937 generator;

    TSP(vector<vector<int>> &d) : distances(d),
                                  n(distances.size()),
                                  generator(rd()) {}

    vector<int> generateInitialSolution()
    {
        vector<int> route(n);
        for (int i = 0; i < n; i++)
        {
            route[i] = i;
        }
        shuffle(route.begin(), route.end(), generator);

        return route;
    }

    int calcTotalCost(vector<int> &route)
    {
        int totalCost = 0;
        for (int i = 0; i < n; i++)
        {
            int a = route[i];
            int b = route[(i + 1) % n];
            totalCost += distances[a][b];
        }
        return totalCost;
    }

    vector<int> bestNeighbour(vector<int> &currRoute, int &bestCost)
    {
        vector<int> &bestRoute = currRoute;
        bestCost = calcTotalCost(currRoute);

        for (int i = 0; i < n - 1; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                vector<int> newRoute = currRoute;
                swap(newRoute[i], newRoute[j]);

                int currCost = calcTotalCost(newRoute);

                if (currCost < bestCost)
                {
                    bestCost = currCost;
                    bestRoute = newRoute;
                }
            }
        }
        return bestRoute;
    }

    pair<vector<int>, int> hillClimbingAlgo(int maxIterations)
    {

        // start with a random solution

        vector<int> currSolution = generateInitialSolution();
        int currCost = calcTotalCost(currSolution);

        for (int i = 0; i < maxIterations; i++)
        {
            int newCost;

            vector<int> newSolution = bestNeighbour(currSolution, newCost);

            if (newCost >= currCost)
            {
                // optima found break here
                break;
            }
            currSolution = newSolution;
            currCost = newCost;
        }

        return {currSolution, currCost};
    }
};

int main()
{
    int numCities;
    cout << "Enter the number of cities\n";
    cin >> numCities;

    vector<vector<int>> distMatrix(numCities, vector<int>(numCities));
    cout << "Enter the value of distance between 2 cities, distance matrix should be symmetric\n";

    for (int i = 0; i < numCities; i++)
    {
        for (int j = 0; j < numCities; j++)
        {
            if (i == j)
                distMatrix[i][j] = 0;

            else
            {
                cout << "Distance from city " << i << " to city " << j << ": ";
                int d;
                cin >> d;
                distMatrix[i][j] = d;
                cout << "\n";
            }
        }
    }

    TSP tsp_solver(distMatrix);

    cout << "Enter the maximum number of iterations:\n";
    int maxIterations;
    cin >> maxIterations;

    pair<vector<int>, int> res = tsp_solver.hillClimbingAlgo(maxIterations);

    vector<int> bestRoute = res.first;
    int bestDist = res.second;

    cout << "Best Route is:\n";

    for (int city : bestRoute)
    {
        cout << city << "-->";
    }

    cout << bestRoute[0] << "\n";

    cout << "Best possible total distance is : " << bestDist << "\n";

    return 0;
}

/*
    It is to note that there can be multiple number of TSP's with same best cost possible for a given Graph
*/