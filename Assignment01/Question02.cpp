/*
    As the problem statement talks of DFS, we need to think what are the nodes and what are the edges first.

    A node here as we have seen in DFA can be thought of a state.

    Here state can be represented as a pair of the current volume of water in the two jugs

    Jug1 has capacity of 3 units while Jug2 has capacity of 4 units

    The edges between nodes can be thought of as transitions described in the question:-
        1. Empty jug1
        2. Empty jug2
        3. Fill jug1
        4. Fill jug2
        5. Pour from jug1 to jug2
        6. Pour from jug2 to jug1

    Initial state as per the question is <0,0>

    The question also mentions a target T, which should be finally present
    in the 4 unit jug after all the transitions.

    Final state will be <k,T>. k can be any arbitrary non-negative number less than 3

    A simple idea will be to start the DFS from the initial state and find whether the final state
    is reachable or not.

*/

#include <bits/stdc++.h>

using namespace std;

class DFSSolver
{
public:
    const int capacity1, capacity2;
    const int target;
    // state is a pair<int,int>p, where p.first=jug1 volume and p.second=jug2 volume
    set<pair<int, int>> visited;                // keep track of visited states to prevent loops
    map<pair<int, int>, pair<int, int>> parent; // To keep track of the previous state
    pair<int, int> targetState;

    DFSSolver(int cap1, int cap2, int t) : capacity1(cap1), capacity2(cap2), target(t)
    {
        targetState = {-1, -1};
    }

    vector<pair<int, int>> nextStates(pair<int, int> &currState)
    {
        vector<pair<int, int>> res;

        // fill jug1
        if (currState.first < capacity1)
        {
            res.push_back({capacity1, currState.second});
        }

        // fill jug2
        if (currState.second < capacity2)
        {
            res.push_back({currState.first, capacity2});
        }

        // empty jug1

        if (currState.first > 0)
        {
            res.push_back({0, currState.second});
        }

        // empty jug2

        if (currState.second > 0)
        {
            res.push_back({currState.first, 0});
        }

        // pour from jug1 to jug2
        if (currState.first > 0 && currState.second < capacity2)
        {
            int pour = min(currState.first, capacity2 - currState.second);
            res.push_back({currState.first - pour, currState.second + pour});
        }

        // pour from jug2 ro jug1

        if (currState.second > 0 && currState.first < capacity1)
        {
            int pour = min(currState.second, capacity1 - currState.first);
            res.push_back({currState.first + pour, currState.second - pour});
        }

        return res;
    }

    bool dfs()
    {
        stack<pair<int, int>> st;
        pair<int, int> initialState = {0, 0};

        st.push(initialState);
        visited.insert(initialState);

        while (!st.empty())
        {
            pair<int, int> curr = st.top();
            st.pop();

            if (curr.second == target)
            {
                targetState = curr;
                return true;
            }

            for (auto neighbour : nextStates(curr))
            {
                if (visited.find(neighbour) == visited.end())
                {
                    visited.insert(neighbour);
                    parent[neighbour] = curr;
                    st.push(neighbour);
                }
            }
        }
        return false;
    }

    void printSequence()
    {
        vector<pair<int, int>> path;
        pair<int, int> curr = targetState;

        int len = 0;

        while (!(curr.first == 0 && curr.second == 0))
        {
            len++;
            path.push_back(curr);
            curr = parent[curr];
        }

        path.push_back({0, 0});
        cout << "Solution exists and has " << len << " number of transitions\n";

        for (int i = path.size() - 1; i >= 0; i--)
        {
            cout << "Step " << path.size() - i << ": ";
            cout << "<" << path[i].first << "," << path[i].second << ">";

            if (i < path.size() - 1)
            {
                pair<int, int> curr = path[i];
                pair<int, int> prev = path[i + 1];

                if (curr.first > prev.first && curr.second == prev.second)
                {
                    cout << " - Fill jug1";
                }
                else if (curr.second > prev.second && curr.first == prev.first)
                {
                    cout << " - Fill jug2";
                }
                else if (curr.first < prev.first && curr.second > prev.second)
                {
                    cout << " - Pour from jug1 to jug2";
                }
                else if (curr.first > prev.first && curr.second < prev.second)
                {
                    cout << " - Pour from jug2 to jug1";
                }
                else if (curr.first == 0 && prev.first > 0)
                {
                    cout << " - Empty jug1";
                }
                else if (curr.second == 0 && prev.second > 0)
                {
                    cout << " - Empty jug2";
                }
            }
            cout << "\n";
        }
    }
};

int main()
{

    DFSSolver solver(9, 5, 3);

    if (solver.dfs())
    {
        solver.printSequence();
    }
    else
    {
        cout << "No solution exists!\n";
    }

    return 0;
}
