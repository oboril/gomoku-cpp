#include "board.hpp"

// EVAL TABLE LAYOUT
//
// 0 - 11 (12): stone counts
// 0: no stones, 1-5: to-play stones, 6: bias, 7-11: just-played stones
//
// 12-19 (8): same in 2 dirs
// 12-15: to-play stones, 16-19: just-played stones
// 00 11 22 33 44
//
// 20-40 (20): different in two directions
// 10 20 21 30 31 32 40 41 42 43
//
// TOTAL 40

// Returns evaluation of the current position
int64_t Board::evaluate(const EvaluationTable *eval_table) const
{
    //              ###############  CREATE THE TABLE ###############
    // x, y, player, count, dir

    uint8_t counts[BOARD_SIZE][BOARD_SIZE][2][8/*6*/][4] = {};
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
            counts[0][x][y][c1][0]++;
            counts[1][x][y][c2][0]++;
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
            counts[0][x][y][c1][1] += (c2 == 0);
            counts[1][x][y][c2][1] += (c1 == 0);
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
            counts[0][x][y][c1][2] += (c2 == 0);
            counts[1][x][y][c2][2] += (c1 == 0);

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
            counts[0][x][y][c1][3] += (c2 == 0);
            counts[1][x][y][c2][3] += (c1 == 0);
        }
    }

    //          ##################   SUM UP THE EVALUATION    ####################
    int64_t eval = (*eval_table)[6]; // BIAS

    const int64_t *counts_eval_table = (int64_t *)&eval_table[0];
    const int64_t *same_pairs_eval_table = (int64_t *)&eval_table[12]; // 00, 11, 22, 33, 44 to play, same just played
    const int64_t *diff_pairs_eval_table = (int64_t *)&eval_table[22]; // different pairs (2*10)

    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            // [player, count, direction]
            auto square = counts[x][y];

            // SIMPLE COUNTS
            for (int dir = 0; dir < 4; dir++)
            {
                eval += square[0][0][dir] * counts_eval_table[0];
                eval += square[0][1][dir] * counts_eval_table[1];
                eval += square[0][2][dir] * counts_eval_table[2];
                eval += square[0][3][dir] * counts_eval_table[3];
                eval += square[0][4][dir] * counts_eval_table[4];
                eval += square[0][5][dir] * counts_eval_table[5];

                eval += square[1][1][dir] * counts_eval_table[6 + 1];
                eval += square[1][2][dir] * counts_eval_table[6 + 2];
                eval += square[1][3][dir] * counts_eval_table[6 + 3];
                eval += square[1][4][dir] * counts_eval_table[6 + 4];
                eval += square[1][5][dir] * counts_eval_table[6 + 5];
            }

            // PAIRS
            for (int p = 0; p < 2; p++)
            {
                for (int dir = 0; dir < 4; dir++)
                {
                    // set all but highest count to 0
                    square[p][0][dir] *= !square[p][1][dir] && !square[p][2][dir] && !square[p][3][dir] && !square[p][4][dir];
                    square[p][1][dir] *= !square[p][2][dir] && !square[p][3][dir] && !square[p][4][dir];
                    square[p][2][dir] *= !square[p][3][dir] && !square[p][4][dir];
                    square[p][3][dir] *= !square[p][4][dir];
                }

                // SAME PAIRS
                for (int c = 0; c < 5; c++)
                {
                    int mask = (((square[p][c][0] > 0) + (square[p][c][1] > 0) + (square[p][c][2] > 0) + (square[p][c][3] > 0)) > 1);
                    eval += same_pairs_eval_table[p*5+c] * mask;
                }

                //DIFFERENT PAIRS
                int idx = 0;
                for (int c1 = 0; c1 < 5; c1++)
                {
                    for (int c2 = 0; c2 < c1; c2++)
                    {
                        idx++;
                        int mask = (((square[p][c1][0] > 0) + (square[p][c1][1] > 0) + (square[p][c1][2] > 0) + (square[p][c1][3] > 0)) > 0);
                        mask *= (((square[p][c2][0] > 0) + (square[p][c2][1] > 0) + (square[p][c2][2] > 0) + (square[p][c2][3] > 0)) > 0);
                        eval += diff_pairs_eval_table[p*10 + idx] * mask;
                    }
                }
            }
        }
    }

    return eval;
}