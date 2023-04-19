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

bool play_move(Board *board, EvaluationTable *eval_table)
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

// Returns RMSE for the predicted move evaluation
double get_eval_errors(Board *board, EvaluationTable *eval_table, EvaluationTable *predict_table)
{
    auto moves = board->get_moves(predict_table);

    double curr_eval = (double)board->evaluate(eval_table);
    double error = 0;

    const double count = (double)moves.size();

    for (const Move m : moves) {
        board->play(m.point);
        double new_eval = -(double)board->evaluate(eval_table);
        board->reset_move(m.point);
        double err = (curr_eval + (double)m.score) - new_eval;
        error += err*err;
    }
    error = sqrt(error / count);
    return error;
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    // vector<size_t> variables{1, 2, 3, 4, 7, 8, 9, 10};
    vector<size_t> variables{1, 2, 3, 4, 7, 8, 9, 10};

    EvaluationTable predict_table;
    memcpy(&predict_table, &DEFAULT_PREDICT_TABLE, sizeof(EvaluationTable));

    EvaluationTable default_eval_table;
    memcpy(&default_eval_table, &DEFAULT_EVAL_TABLE, sizeof(EvaluationTable));

    double predict_table_double[12];
    for (int i = 0; i < 12; i++)
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
        if (play_move(&b, &default_eval_table) || board_moves > 10)
        {
            b = random_board(4);
            board_moves = 0;
        }
        double rmse = get_eval_errors(&b, &default_eval_table, &predict_table);

        //if (rmse > 10000000) {
        //    b = random_board(4);
        //    board_moves = 0;
        //    iter--;
        //    continue;
        //}

        mean_mse += rmse / 100000;

        if (iter % 100000 == 0)
        {
            std::cout << "RMSE: " << mean_mse << "\n";

            if (iter % 100000 == 0)
            {
                std::cout << "Predict table: ";
                for (int i = 0; i < 12; i++)
                {
                    std::cout << predict_table[i] << ", ";
                }
                std::cout << "\n\n";
            }

            mean_mse = 0;
        }

        constexpr double MOMENTUM = 0.5;

        // accumulate gradient
        for (size_t var : variables)
        {
            int64_t *v = &predict_table[var];
            *v += 1;
            double rmse_new = get_eval_errors(&b, &default_eval_table, &predict_table);
            double grad = rmse - rmse_new;
            *v -= 1;

            gradients[var] += grad;// * (1-MOMENTUM);
        }

        // apply gradient to eval table
        if (iter % 10000 == 0)
        {
            for (size_t var : variables)
            {
                predict_table_double[var] += gradients[var] / 10000 * 100;
                gradients[var] *= MOMENTUM;
            }

            for (int i = 0; i < 12; i++)
            {
                predict_table[i] = (int64_t)predict_table_double[i];
            }
        }
    }

    return 0;
}