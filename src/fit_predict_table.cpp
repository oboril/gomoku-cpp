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

// Returns error for the predicted move evaluation
double get_eval_errors(Board *board, const EvaluationTable *eval_table, const EvaluationTable *predict_table)
{
    // best move should leave eval unchanged
    // predicted = current + move_prediction

    auto moves = board->get_moves(predict_table);

    double curr_eval = (double)board->evaluate(eval_table);

    double error = 0;

    // sum all errors
    for (const Move m : moves)
    {
        board->play(m.point);
        double new_eval = -(double)board->evaluate(eval_table);
        board->reset_move(m.point);

        double predicted_eval = curr_eval + (double)m.score;

        double err = predicted_eval - new_eval;
        error += err * err;
    }
    error = sqrt(error / (double)moves.size());
    return error;
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    constexpr double learning_rates[EVAL_TABLE_SIZE] = {1e3, 5e3, 1e4, 5e4, 1e12, 0, 1e4,5e3, 1e4, 3e4, 1e7, 0};
    constexpr double LR = 30e-3;
    constexpr double MOMENTUM = 0.9;

    EvaluationTable predict_table{150, 6000, 12000, 29000, WIN, WIN, -3500, 500, 5000, 15000, 10000000, LOSS};

    double predict_table_double[EVAL_TABLE_SIZE];
    for (int i = 0; i < EVAL_TABLE_SIZE; i++)
    {
        predict_table_double[i] = (double)predict_table[i];
    }

    Board b = random_board(4);
    double mean_mse = 0;
    int board_moves = 0;
    double gradients[12] = {};
    for (int iter = 0; iter < 1000000000; iter++)
    {
        board_moves += 1;
        if (play_move(&b, &DEFAULT_PREDICT_TABLE) || board_moves > 10)
        {
            b = random_board(4);
            board_moves = 0;
        }
        double rmse = get_eval_errors(&b, &DEFAULT_EVAL_TABLE, &predict_table);

        mean_mse += rmse / 10000;

        if (iter % 10000 == 0)
        {
            std::cout << "RMSE: " << mean_mse << "\n";
            std::cout << "Predict table: ";
            for (int i = 0; i < 12; i++)
            {
                std::cout << predict_table[i] << ", ";
            }
            std::cout << "\n\n";
            mean_mse = 0;
        }

        // accumulate gradient
        for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
        {
            int64_t *v = &predict_table[var];
            *v += 1;
            double rmse_new = get_eval_errors(&b, &DEFAULT_EVAL_TABLE, &predict_table);
            double grad = rmse - rmse_new;
            *v -= 1;

            gradients[var] += grad; // * (1-MOMENTUM);
        }

        // apply gradient to eval table
        if (iter % 1000 == 0)
        {
            for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
            {
                // update precise values
                predict_table_double[var] += gradients[var] / 1000 * LR * learning_rates[var];
                gradients[var] *= MOMENTUM;

                // update integer values
                predict_table[var] = (int64_t)predict_table_double[var];
            }
        }
    }

    return 0;
}