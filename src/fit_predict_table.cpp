#include <iostream>
#include "board.hpp"
#include "minimax.hpp"
#include "timer.hpp"
#include <random>
#include <vector>
#include "error_transform.hpp"
using namespace std;

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

        double real_change = transform_value(curr_eval - new_eval);
        double pred_change = transform_value(m.score);

        error += abs(real_change - pred_change);
    }

    return error;
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    EvaluationTable predict_table{400, 8000, 16000, 20000, WIN/100, WIN, 0, 1500, 8500, 20000, WIN/1000, LOSS};
    constexpr double learning_rates[EVAL_TABLE_SIZE] = {1e3, 5e3, 1e4, 5e4, 1e13, 0, 1e3,1e3, 1e4, 2e4, 1e12, 0};
    constexpr double LR = 10;
    constexpr double MOMENTUM = 0.7;
    constexpr int BATCH = 300;


    double predict_table_double[EVAL_TABLE_SIZE];
    for (int i = 0; i < EVAL_TABLE_SIZE; i++)
    {
        predict_table_double[i] = (double)predict_table[i];
    }

    Board b = Board::random(2);
    double mean_mse = 0;
    int board_moves = 0;
    double gradients[12] = {};
    for (int iter = 0; iter < 1000000000; iter++)
    {
        board_moves += 1;
        if (b.play_predicted_move(&predict_table) || board_moves > 10)
        {
            b = Board::random(2);
            board_moves = 0;
        }
        double rmse = get_eval_errors(&b, &DEFAULT_EVAL_TABLE, &predict_table);

        mean_mse += rmse / (BATCH*10);

        if (iter % (BATCH*10) == 0)
        {
            std::cout << "RMSE: " << mean_mse << "\n";
            std::cout << "Predict table: ";
            for (int i = 0; i < 12; i++)
            {
                std::cout << predict_table[i] << ", ";
            }
            std::cout << "\n" << endl;
            b.print();
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
        if (iter % BATCH == 0)
        {
            for (size_t var = 0; var < EVAL_TABLE_SIZE; var++)
            {
                // update precise values
                predict_table_double[var] += gradients[var] / BATCH * LR * learning_rates[var];
                gradients[var] *= MOMENTUM;

                // update integer values
                predict_table[var] = (int64_t)predict_table_double[var];
            }
        }
    }

    return 0;
}