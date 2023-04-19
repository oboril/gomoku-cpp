#include <iostream>
#include "board.hpp"
#include "minimax.hpp"
#include "timer.hpp"
#include <random>
#include <vector>
using namespace std;

Board random_board(int iters)
{
    Board b;
    while (iters > 0)
    {
        int x = (std::rand() % 4) + 1;
        int y = (std::rand() % 4) + 1;
        if (b.play_checked(Point(x, y)))
        {
            iters--;
        }
    }
    return b;
}

bool play_move(Board *board, const EvaluationTable *eval_table)
{
    auto moves = board->get_moves(eval_table);
    Move best_move(Point(0, 0), -1);
    for (Move m : moves)
    {
        if (m.score > best_move.score)
        {
            best_move = m;
        }
    }
    board->play(best_move.point);
    return board->check_win(best_move.point);
}

double get_error(Board *board, const EvaluationTable *eval_table)
{
    double e1 = (double)board->evaluate(eval_table);
    int64_t iters = 0;
    double e2 = (double)negamax_ab_ordered(*board, 1, eval_table, &DEFAULT_PREDICT_TABLE, &iters);
    double err = (e1-e2)/MAX(abs(e1), abs(e2));
    return abs(err);
    // USING RELATIVE ERROR SEEMS VERY NICE
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    EvaluationTable eval_table{30, 400, 10000, 25000, 10000000, WIN, 10000, -100, -8000, -20000, -40000, LOSS};
    constexpr double learning_rates[EVAL_TABLE_SIZE] = {0.001,0.01,0.5,5.,50,0,50,0.1,0.5,5.,50,0};
    constexpr double LR = 10e5;
    constexpr double MOMENTUM = 0.9;

    double eval_table_double[12];
    for (int i = 0; i < 12; i++)
    {
        eval_table_double[i] = (double)eval_table[i];
    }

    Board b = random_board(4);
    double mean_mse = 0;
    int board_moves = 0;
    double gradients[12] = {};
    for (int iter = 0; iter < 1000000000; iter++)
    {
        board_moves += 1;
        if (play_move(&b, &DEFAULT_PREDICT_TABLE) || board_moves > 50)
        {
            b = random_board(4);
            board_moves = 0;
        }
        double rmse = get_error(&b, &eval_table);

        if (rmse < 30000) {
            mean_mse += rmse / 10000;
        }
        

        if (iter % 10000 == 0)
        {
            std::cout << "MSE: " << mean_mse << "\n";

            std::cout << "Eval table: ";
            for (int i = 0; i < 12; i++)
            {
                std::cout << eval_table[i] << ", ";
            }
            std::cout << "\n\n";

            mean_mse = 0;
        }

        // accumulate gradient
        for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
        {
            int64_t *v = &eval_table[var];
            *v += 1;
            double rmse2 = get_error(&b, &eval_table);
            double grad = rmse - rmse2;
            *v -= 1;

            gradients[var] += grad;
        }

        // apply gradient to eval table
        if (iter % 1000 == 0)
        {
            for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
            {
                eval_table_double[var] += gradients[var] / 1000 * learning_rates[var] * LR;
                gradients[var] *= MOMENTUM;

                eval_table[var] = (int64_t)eval_table_double[var];
            }
        }
    }

    return 0;
}