#include "board.hpp"
#include "minimax.hpp"
#include <iostream>
#include "adam.hpp"
#include "error_transform.hpp"
using std::cout;
using std::endl;

// Board generation: generate 3 random points, then play best move on depth 2
// Use the board to calculate gradients for predict table
// Use the evaluation to calculate gradients for eval table
// Once a while, use gradients to update values

double get_eval_error(double e1, double e2, double e3)
{
    // Importance scaling, most sensitivity is around 10k-100k
    e1 = transform_value(e1);
    e2 = transform_value(e2);
    e3 = transform_value(e3);

    double mean = (e1 + e2 + e3) / 3;
    double err = abs(mean - e1) + abs(mean - e2) + abs(mean - e3);
    return err / 3;
}

double get_pred_error(Board *board, const EvaluationTable *eval_table, const EvaluationTable *predict_table)
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
    constexpr EvaluationTable eval_init = {100, 1500, 20000, 90000, WIN / 100, WIN, 10000, -500, -20000, -70000, -250000, LOSS};
    constexpr EvaluationTable pred_init = {400, 8000, 16000, 1000000, WIN / 10, WIN, -2000, 1500, 8500, 100000, WIN / 1000, LOSS};

    constexpr double LEARNING_RATE = 0.01;
    constexpr int BATCH_SIZE = 100;
    #define NEW_BOARD() Board::random(3)

    AdamOpt<12> eval_opt((int64_t *)&eval_init);
    AdamOpt<12> pred_opt((int64_t *)&pred_init);

    Board board = NEW_BOARD();
    int board_moves = 0;
    int64_t cumul_iters = 0;
    double cumul_eval_error = 0;
    double cumul_pred_error = 0;
    Point next_move;

    for (int iter = 0; iter >= 0; iter++)
    {
        // get board evaluation
        EvaluationTable *eval_table = (EvaluationTable *)&eval_opt.vals;
        EvaluationTable *pred_table = (EvaluationTable *)&pred_opt.vals;
        double eval0 = board.evaluate(eval_table);
        double eval1 = negamax_predict(board, 1, eval_table, pred_table, &cumul_iters, &next_move);
        double eval2 = negamax_predict(board, 2, eval_table, pred_table, &cumul_iters, &next_move);
        double eval_loss = get_eval_error(eval0, eval1, eval2);

        // get eval gradients
        for (int i = 0; i < EVAL_TABLE_SIZE; i++)
        {
            (*eval_table)[i]++;
            double e0 = board.evaluate(eval_table);
            double e1 = negamax_predict(board, 1, eval_table, pred_table, &cumul_iters, &next_move);
            double e2 = negamax_predict(board, 2, eval_table, pred_table, &cumul_iters, &next_move);
            (*eval_table)[i]--;

            double new_loss = get_eval_error(e0, e1, e2);
            eval_opt.grad[i] += eval_loss - new_loss;
        }

        // get prediction loss
        double pred_loss = get_pred_error(&board, eval_table, pred_table);

        // get eval gradients
        for (int i = 0; i < EVAL_TABLE_SIZE; i++)
        {
            (*pred_table)[i]++;
            double new_loss = get_pred_error(&board, eval_table, pred_table);
            (*pred_table)[i]--;

            pred_opt.grad[i] += pred_loss - new_loss;
        }

        // update values when batch is done
        if (iter % BATCH_SIZE == 0)
        {
            pred_opt.apply_gradient(LEARNING_RATE);
            eval_opt.apply_gradient(LEARNING_RATE);
        }

        // update stats
        cumul_eval_error += eval_loss;
        cumul_pred_error += pred_loss;
        // print summary
        if (iter % (BATCH_SIZE * 10) == 0)
        {
            cout << "Iteration: " << iter;
            cout << ": eval_loss " << (cumul_eval_error / (BATCH_SIZE * 10));
            cout << ", pred_loss " << (cumul_pred_error / (BATCH_SIZE * 10));
            cout << ", negamax_iters " << (cumul_iters / (BATCH_SIZE * 10)) << "\n";
            cout << "Eval table: ";
            for (int i = 0; i < EVAL_TABLE_SIZE; i++)
            {
                cout << eval_opt.vals[i] << ", ";
            }
            cout << "\nPred table: ";
            for (int i = 0; i < EVAL_TABLE_SIZE; i++)
            {
                cout << pred_opt.vals[i] << ", ";
            }
            cout << "\n"
                 << endl;

            cumul_eval_error = 0;
            cumul_pred_error = 0;
            cumul_iters = 0;
        }

        // update board
        board.play(next_move);
        if (board.check_win(next_move) || board_moves > 50)
        {
            board = NEW_BOARD();
            board_moves = 0;
        }

        board.debug();
    }
    return 0;
};