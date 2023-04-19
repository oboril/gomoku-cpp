#include "board.hpp"
#include <iostream>
using std::vector;

inline int64_t eval_counts(BoardRow c1, BoardRow c2, const EvaluationTable *eval_table)
{
    if (c2 == 0)
    {
        return (*eval_table)[c1];
    }
    else if (c1 == 0)
    {
        return (*eval_table)[c2 + 6];
    }
    else
    {
        return 0;
    }
}

// Returns valid moves and their evaluated score
vector<Move> Board::get_moves(const EvaluationTable *eval_table) const
{
    int64_t eval[BOARD_SIZE][BOARD_SIZE] = {};

    const BoardRow *p1 = m_board[m_player];
    const BoardRow *p2 = m_board[!m_player];

    // ROWS
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE - 4; x++)
        {
            BoardRow c1 =
                ((p1[y] >> (x + 0)) & 1) +
                ((p1[y] >> (x + 1)) & 1) +
                ((p1[y] >> (x + 2)) & 1) +
                ((p1[y] >> (x + 3)) & 1) +
                ((p1[y] >> (x + 4)) & 1);
            BoardRow c2 =
                ((p2[y] >> (x + 0)) & 1) +
                ((p2[y] >> (x + 1)) & 1) +
                ((p2[y] >> (x + 2)) & 1) +
                ((p2[y] >> (x + 3)) & 1) +
                ((p2[y] >> (x + 4)) & 1);

            int64_t e = eval_counts(c1, c2, eval_table);
            eval[x + 0][y] += e;
            eval[x + 1][y] += e;
            eval[x + 2][y] += e;
            eval[x + 3][y] += e;
            eval[x + 4][y] += e;
        }
    }

    // COLUMNS
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE - 4; y++)
        {
            BoardRow c1 =
                ((p1[y + 0] >> x) & 1) +
                ((p1[y + 1] >> x) & 1) +
                ((p1[y + 2] >> x) & 1) +
                ((p1[y + 3] >> x) & 1) +
                ((p1[y + 4] >> x) & 1);
            BoardRow c2 =
                ((p2[y + 0] >> x) & 1) +
                ((p2[y + 1] >> x) & 1) +
                ((p2[y + 2] >> x) & 1) +
                ((p2[y + 3] >> x) & 1) +
                ((p2[y + 4] >> x) & 1);

            int64_t e = eval_counts(c1, c2, eval_table);
            eval[x][y + 0] += e;
            eval[x][y + 1] += e;
            eval[x][y + 2] += e;
            eval[x][y + 3] += e;
            eval[x][y + 4] += e;
        }
    }

    // DIAGONALS
    for (int x = 0; x < BOARD_SIZE - 4; x++)
    {
        for (int y = 0; y < BOARD_SIZE - 4; y++)
        {
            // MAIN DIAGONAL
            BoardRow c1 =
                ((p1[y + 0] >> (x + 0)) & 1) +
                ((p1[y + 1] >> (x + 1)) & 1) +
                ((p1[y + 2] >> (x + 2)) & 1) +
                ((p1[y + 3] >> (x + 3)) & 1) +
                ((p1[y + 4] >> (x + 4)) & 1);
            BoardRow c2 =
                ((p2[y + 0] >> (x + 0)) & 1) +
                ((p2[y + 1] >> (x + 1)) & 1) +
                ((p2[y + 2] >> (x + 2)) & 1) +
                ((p2[y + 3] >> (x + 3)) & 1) +
                ((p2[y + 4] >> (x + 4)) & 1);
            int64_t e = eval_counts(c1, c2, eval_table);
            eval[x + 0][y + 0] += e;
            eval[x + 1][y + 1] += e;
            eval[x + 2][y + 2] += e;
            eval[x + 3][y + 3] += e;
            eval[x + 4][y + 4] += e;

            // OTHER DIAGONAL
            c1 =
                ((p1[y + 0] >> (x + 4)) & 1) +
                ((p1[y + 1] >> (x + 3)) & 1) +
                ((p1[y + 2] >> (x + 2)) & 1) +
                ((p1[y + 3] >> (x + 1)) & 1) +
                ((p1[y + 4] >> (x + 0)) & 1);
            c2 =
                ((p2[y + 0] >> (x + 4)) & 1) +
                ((p2[y + 1] >> (x + 3)) & 1) +
                ((p2[y + 2] >> (x + 2)) & 1) +
                ((p2[y + 3] >> (x + 1)) & 1) +
                ((p2[y + 4] >> (x + 0)) & 1);
            e = eval_counts(c1, c2, eval_table);
            eval[x + 4][y + 0] += e;
            eval[x + 3][y + 1] += e;
            eval[x + 2][y + 2] += e;
            eval[x + 1][y + 3] += e;
            eval[x + 0][y + 4] += e;
        }
    }

    // Put all moves and scores into vector
    const int64_t bias = (*eval_table)[6];
    std::vector<Move> moves;
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            Point p(x, y);
            if (is_empty(p))
            {
                moves.push_back(Move(p, eval[x][y] + bias));
            }
        }
    }

    return moves;
}