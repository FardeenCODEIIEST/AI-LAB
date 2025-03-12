#include <bits/stdc++.h>

#define ROWS 6
#define COLS 7
#define PLAYER 1
#define AI 2
#define EMPTY 0

using namespace std;

class Connect4
{
public:
    vector<vector<int>> board;

    Connect4()
    {
        board = vector<vector<int>>(ROWS, vector<int>(COLS, EMPTY));
    }

    void printBoard()
    {
        // Prints the current board configuration
        for (int i = 0; i < ROWS; i++)
        {
            cout << "\n-----------------------------\n| ";

            for (int j = 0; j < COLS; j++)
            {
                if (board[i][j] == PLAYER)
                    cout << "x" << " | ";
                else if (board[i][j] == AI)
                    cout << "o" << " | ";
                else
                    cout << " " << " | ";
            }
        }
        cout << "\n-----------------------------\n";
    }

    bool isColumnFull(int col)
    {
        // Returns true if column is fully occupied
        return board[0][col] != EMPTY;
    }

    bool placePiece(int col, int player)
    {
        // Returns true if the piece can be placed
        for (int i = ROWS - 1; i >= 0; i--)
        {
            if (board[i][col] == EMPTY)
            {
                board[i][col] = player;
                return true;
            }
        }
        return false;
    }

    bool checkAlignment(int r, int c, int dr, int dc, int player)
    {
        /* Returns true if starting from the given (r,c) following
         the trend (dr,dc), we get a length of atleast 4*/
        int cnt = 0;
        for (int i = 0; i < 4; i++)
        {
            int nr = r + i * dr;
            int nc = c + i * dc;
            if (nr >= 0 && nc >= 0 && nr < ROWS && nc < COLS && board[nr][nc] == player)
            {
                cnt++;
            }
            else
            {
                break;
            }
        }
        return cnt == 4;
    }

    bool checkWin(int player)
    {
        // Returns true if the player wins

        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (board[i][j] == player)
                {
                    // horizontal
                    bool hori_flag = checkAlignment(i, j, 0, 1, player);
                    // vertical
                    bool vert_flag = checkAlignment(i, j, 1, 0, player);
                    // main diag
                    bool main_diag = checkAlignment(i, j, 1, 1, player);
                    // off diag
                    bool off_diag = checkAlignment(i, j, 1, -1, player);

                    if (hori_flag || vert_flag || main_diag || off_diag)
                        return true;
                }
            }
        }
        return false;
    }

    bool isBoardFull()
    {
        // Returns true if board is full

        for (int i = 0; i < COLS; i++)
        {
            if (!isColumnFull(i))
                return false;
        }
        return true;
    }

    string boardHash()
    {
        // Returns a hash-string for the given board config.
        string key = "";
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                key += to_string(board[i][j]);
            }
        }
        return key;
    }
};

// Consider AI as the maximiser and player as the minimiser
class MiniMaxSolver
{
public:
    unordered_map<string, int> memo;

    int evaluateBoard(Connect4 &game)
    {
        // Evaluation of terminal nodes in the game tree
        /*
            TODO -> Can be optimized to improve the optimality of action
        */
        if (game.checkWin(AI))
        {
            return 1000;
        }
        else if (game.checkWin(PLAYER))
        {
            return -1000;
        }

        // Immediate Threat from player
        for (int i = 0; i < COLS; i++)
        {
            if (!game.isColumnFull(i))
            {
                Connect4 newState = game;
                newState.placePiece(i, PLAYER);
                if (newState.checkWin(PLAYER))
                {
                    return -500;
                }
            }
        }

        // Immediate threat from AI
        for (int i = 0; i < COLS; i++)
        {
            if (!game.isColumnFull(i))
            {
                Connect4 newState = game;
                newState.placePiece(i, AI);
                if (newState.checkWin(AI))
                {
                    return 500;
                }
            }
        }
        return 0;
    }

    int minimax(Connect4 &game, int depth, int alpha, int beta, bool isMaximizer)
    {
        // EValuates the value of the root node in the game tree keeping AI as maximiser
        string board_hash = game.boardHash() + (isMaximizer ? "A" : "P");

        if (memo.find(board_hash) != memo.end())
        {
            return memo[board_hash];
        }

        if (depth == 0 || game.checkWin(AI) || game.checkWin(PLAYER) || game.isBoardFull())
        {
            int eval = evaluateBoard(game);
            memo[board_hash] = eval;
            return eval;
        }

        if (isMaximizer)
        {
            int maxEval = INT_MIN;
            for (int i = 0; i < COLS; i++)
            {
                if (!game.isColumnFull(i))
                {
                    Connect4 newState = game;
                    newState.placePiece(i, AI);
                    int eval = minimax(newState, depth - 1, alpha, beta, false);
                    maxEval = max(maxEval, eval);
                    alpha = max(alpha, eval);
                    if (alpha >= beta)
                    {
                        // prune;
                        break;
                    }
                }
            }
            memo[board_hash] = maxEval;
            return maxEval;
        }
        else
        {
            int minEval = INT_MAX;
            for (int i = 0; i < COLS; i++)
            {
                if (!game.isColumnFull(i))
                {
                    Connect4 newState = game;
                    newState.placePiece(i, PLAYER);
                    int eval = minimax(newState, depth - 1, alpha, beta, true);
                    minEval = min(minEval, eval);
                    beta = min(beta, eval);
                    if (alpha >= beta)
                    {
                        // prune
                        break;
                    }
                }
            }
            memo[board_hash] = minEval;
            return minEval;
        }
    }

    int optimalAction(Connect4 &game, int depth)
    {
        // Returns optimal Action from a given board state
        int optimalAction = -1;
        int optimalValue = INT_MIN;
        for (int i = 0; i < COLS; i++)
        {
            if (!game.isColumnFull(i))
            {
                Connect4 newState = game;
                newState.placePiece(i, AI);
                int moveVal = minimax(newState, depth - 1, INT_MIN, INT_MAX, false);
                if (moveVal > optimalValue)
                {
                    optimalValue = moveVal;
                    optimalAction = i;
                }
            }
        }
        return optimalAction;
    }
};

int main()
{
    Connect4 game;
    MiniMaxSolver ai_agent;
    int depth; // Max depth of the game tree
    cout << "Your symbol -> x\n";
    cout << "AI's symbol -> o\n";
    cout << "Enter the depth of the Game Tree: ";
    cin >> depth;
    while (true)
    {
        game.printBoard();

        int playerCol;
        cout << "Enter your move (1-7): ";
        cin >> playerCol;
        playerCol--;
        if (playerCol < 0 || playerCol >= COLS || game.isColumnFull(playerCol))
        {
            cout << "Invalid mode, try again\n";
            continue;
        }

        game.placePiece(playerCol, PLAYER);
        if (game.checkWin(PLAYER))
        {
            game.printBoard();
            cout << "You win!\n";
            return 0;
        }
        if (game.isBoardFull())
        {
            game.printBoard();
            cout << "It's a tie\n";
            return 0;
        }

        int aiCol = ai_agent.optimalAction(game, depth);
        game.placePiece(aiCol, AI);
        if (game.checkWin(AI))
        {
            game.printBoard();
            cout << "AI wins!\n";
            return 0;
        }
        if (game.isBoardFull())
        {
            game.printBoard();
            cout << "It's a tie\n";
            return 0;
        }
    }
    return 0;
}
