#include <iostream>
#include "board.hpp"
#include "minimax.hpp"
#include "timer.hpp"
#include <random>
#include <vector>
#include "error_transform.hpp"
using namespace std;

double get_error(Board *board, const EvaluationTable *eval_table)
{
    double e1 = (double)board->evaluate(eval_table);
    int64_t iters = 0;
    double e2 = (double)negamax_ab_ordered(*board, 1, eval_table, &DEFAULT_PREDICT_TABLE, &iters);
    double e3 = (double)negamax_ab_ordered(*board, 2, eval_table, &DEFAULT_PREDICT_TABLE, &iters);

    // Importance scaling, most sensitivity is around 10k-100k
    e1 = transform_value(e1);
    e2 = transform_value(e2);
    e3 = transform_value(e3);

    double mean = (e1 + e2 + e3) / 3;
    double err = abs(mean - e1) + abs(mean - e2) + abs(mean - e3);
    return err/3;
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    EvaluationTable eval_table{-100, 2500, 18000, 80000, WIN/100, WIN, 8500, -500, -13000, -45000, -300000, LOSS};
    constexpr double learning_rates[EVAL_TABLE_SIZE] = {10, 1e3, 1e4, 3e4, 0, 0, 1e4, 10, 1e3, 2e4, 1e6, 0};
    constexpr double LR = 1e4;
    constexpr double MOMENTUM = 0.5;
    constexpr int BATCH = 300;

    double eval_table_double[12];
    for (int i = 0; i < 12; i++)
    {
        eval_table_double[i] = (double)eval_table[i];
    }

    Board b = Board::random(4);
    double mean_mse = 0;
    int board_moves = 0;
    double gradients[12] = {};
    for (int iter = 0; iter < 1000000000; iter++)
    {
        board_moves += 1;
        if (b.play_predicted_move(&DEFAULT_PREDICT_TABLE) || board_moves > 50)
        {
            b = Board::random(4);
            board_moves = 0;
        }
        double rmse = get_error(&b, &eval_table);

        mean_mse += rmse / (BATCH * 10);

        if (iter % (BATCH * 10) == 0)
        {
            std::cout << "MSE: " << mean_mse << "\n";

            std::cout << "Eval table: ";
            for (int i = 0; i < 12; i++)
            {
                std::cout << eval_table[i] << ", ";
            }
            std::cout << "\n" << endl;

            mean_mse = 0;
            b.print();
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
        if (iter % BATCH == 0)
        {
            for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
            {
                eval_table_double[var] += gradients[var] / BATCH * learning_rates[var] * LR;
                gradients[var] *= MOMENTUM;

                eval_table[var] = (int64_t)eval_table_double[var];
            }
        }
    }

    return 0;
}