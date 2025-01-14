#include <bits/stdc++.h>

using namespace std;

/*
    State information contains the :-
        1. board
        2. empty coordinates
        3. g(state)
        4. h(state)
        5. path to reach the goal state

*/
struct State
{
    vector<vector<int>> board;
    int emptyX, emptyY;   // empty cell coordinates
    int g, h;             // g: cost to reach this state, h: heuristic value
    vector<State *> path; // path to reach the goal state

    State(vector<vector<int>> b) : board(b), g(0), h(0)
    {
        // Find empty cell position
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] == 0)
                {
                    emptyX = i;
                    emptyY = j;
                }
            }
        }
    }

    bool operator<(const State &other) const
    {
        return (g + h) > (other.g + other.h); // For priority queue
    }
};

// Calculating the number of misplaced tiles (Heuristic)
int getMisplacedCount(const vector<vector<int>> &current, const vector<vector<int>> &goal)
{
    int count = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (current[i][j] != 0 && current[i][j] != goal[i][j])
            {
                count++;
            }
        }
    }
    return count;
}

// Calculating the Manhattan distance (Heuristic)
int getManhattanDistance(const vector<vector<int>> &current, const vector<vector<int>> &goal)
{
    int distance = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (current[i][j] != 0)
            {
                // Find this number's position in goal state
                for (int x = 0; x < 3; x++)
                {
                    for (int y = 0; y < 3; y++)
                    {
                        if (goal[x][y] == current[i][j])
                        {
                            distance += abs(x - i) + abs(y - j);
                        }
                    }
                }
            }
        }
    }
    return distance;
}

// Flatten the board to string for checking visited states
string boardToString(const vector<vector<int>> &board)
{
    string s;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            s += to_string(board[i][j]);
    return s;
}

// MOve validity
bool isValid(int x, int y)
{
    return x >= 0 && x < 3 && y >= 0 && y < 3;
}

// Solve 8-puzzle using A* search algorithm
vector<State *> solvePuzzle(const vector<vector<int>> &initial, const vector<vector<int>> &goal, bool useManhattan)
{
    // Possible moves (up, right, down, left)
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    priority_queue<State> pq;
    set<string> visited;

    State *initialState = new State(initial);
    initialState->h = useManhattan ? getManhattanDistance(initial, goal) : getMisplacedCount(initial, goal);

    pq.push(*initialState);
    visited.insert(boardToString(initial));

    while (!pq.empty())
    {
        State current = pq.top();
        pq.pop();

        // Check if goal reached
        if (current.board == goal)
        {
            return current.path;
        }

        // All possible moves for the empty cell
        for (int i = 0; i < 4; i++)
        {
            int newX = current.emptyX + dx[i];
            int newY = current.emptyY + dy[i];

            if (isValid(newX, newY))
            {
                vector<vector<int>> newBoard = current.board;
                swap(newBoard[current.emptyX][current.emptyY],
                     newBoard[newX][newY]);

                string boardStr = boardToString(newBoard);
                if (visited.find(boardStr) == visited.end())
                {
                    State *newState = new State(newBoard);
                    newState->g = current.g + 1; // actual cost (edge cost is 1 uniform)
                    // heuristic cost
                    newState->h = useManhattan ? getManhattanDistance(newBoard, goal) : getMisplacedCount(newBoard, goal);
                    newState->path = current.path;
                    newState->path.push_back(newState);

                    pq.push(*newState);
                    visited.insert(boardStr);
                }
            }
        }
    }

    return vector<State *>(); // No solution found
}

// Print board state
void printBoard(const vector<vector<int>> &board)
{
    cout << "--------------\n";
    for (int i = 0; i < 3; i++)
    {
        cout << "| ";
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == 0)
                cout << "   |";
            else
                cout << " " << board[i][j] << " |";
        }
        cout << "\n--------------\n";
    }
    cout << "\n";
}

int main()
{
    vector<vector<int>> initial = {
        {1, 2, 3},
        {8, 0, 4},
        {7, 6, 5}};

    vector<vector<int>> goal = {
        {2, 8, 1},
        {0, 4, 3},
        {7, 6, 5}};

    cout << "Initial State:\n";
    printBoard(initial);
    cout << "Goal State:\n";
    printBoard(goal);

    cout << "Solving using Misplaced Tiles heuristic...\n";
    vector<State *> solution1 = solvePuzzle(initial, goal, false);
    if (!solution1.empty())
    {
        cout << "Solution found in " << solution1.size() << " moves!\n";
        for (State *state : solution1)
        {
            printBoard(state->board);
        }
    }

    cout << "Solving using Manhattan Distance heuristic...\n";
    vector<State *> solution2 = solvePuzzle(initial, goal, true);
    if (!solution2.empty())
    {
        cout << "Solution found in " << solution2.size() << " moves!\n";
        for (State *state : solution2)
        {
            printBoard(state->board);
        }
    }

    return 0;
}