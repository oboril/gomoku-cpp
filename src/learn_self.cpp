#include "board.hpp"
#include "minimax.hpp"
#include <iostream>
#include <chrono>
#include "adam.hpp"
#include "error_transform.hpp"
using std::cout;
using std::endl;

// Board generation: generate 3 random points, then play best move on depth 2
// Use the board to calculate gradients for predict table
// Use the evaluation to calculate gradients for eval table
// Once a while, use gradients to update values

double get_eval_error(double e1, double e2, double e3, double e4)
{
    // Importance scaling, most sensitivity is around 10k-100k
    e1 = transform_value(e1);
    e2 = transform_value(e2);
    e3 = transform_value(e3);
    e4 = transform_value(e4);

    // make even weights for odd and even moves
    double mean = (e1 + e2 + e3 + e4) / 4.;
    double err = std::pow(mean - e1, 2) + std::pow(mean - e2, 2) + std::pow(mean - e3, 2) + std::pow(mean - e4, 2);
    return err / 4.;
}

double get_pred_error(Board *board, const EvaluationTable *eval_table, const EvaluationTable *predict_table)
{
    // best move should leave eval unchanged
    // predicted = current + move_prediction

    auto moves = board->get_moves(predict_table);

    double curr_eval = (double)board->evaluate(eval_table);

    double error = 0.;

    // sum all errors
    for (const Move m : moves)
    {
        board->play(m.point);
        double new_eval = -(double)board->evaluate(eval_table);
        board->reset_move(m.point);

        double real_change = transform_value(new_eval - curr_eval);
        double pred_change = transform_value(m.score);

        error += std::pow(real_change - pred_change, 2);
    }

    return error/(double) moves.size();
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    constexpr EvaluationTable eval_init = {0, 100, 1000, 10000, 100000, WIN, 0, -100, -1000, -10000, -100000, LOSS};
    constexpr EvaluationTable pred_init = {0, 100, 1000, 10000, 100000, WIN, 0, 100, 1000, 10000, 100000, LOSS};

    //#define RN (rand()%10000-5000)
    //const EvaluationTable eval_init = {RN, RN, RN, RN, RN, WIN, RN, RN, RN, RN, RN, LOSS};
    //const EvaluationTable pred_init = {RN, RN, RN, RN, RN, WIN, RN, RN, RN, RN, RN, LOSS};
    //#undef RN

    double LEARNING_RATE = 0.1;
    constexpr int BATCH_SIZE = 300;
    constexpr int PRINT_EVERY = 10;
#define NEW_BOARD() Board::random(3)

    AdamOpt<12> eval_opt((int64_t *)&eval_init);
    AdamOpt<12> pred_opt((int64_t *)&pred_init);

    Board board = NEW_BOARD();
    int board_moves = 0;
    int64_t cumul_iters = 0;
    double cumul_eval_error = 0.;
    double cumul_pred_error = 0.;
    int pos_evals = 0.;
    int neg_evals = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    Point next_move;

    for (int iter = 0; iter >= 0; iter++)
    {
        // adjust learning rate
        if (iter < 100000) {LEARNING_RATE = 0.1;}
        else if (iter < 300000) {LEARNING_RATE = 0.03;}
        else {LEARNING_RATE = 0.01;}
        // get board evaluation
        EvaluationTable *eval_table = (EvaluationTable *)&eval_opt.vals;
        EvaluationTable *pred_table = (EvaluationTable *)&pred_opt.vals;
        double eval0 = (double)board.evaluate(eval_table);
        double eval1 = (double)negamax::predict(board, 1, eval_table, pred_table, &cumul_iters, &next_move);
        double eval2 = (double)negamax::predict(board, 2, eval_table, pred_table, &cumul_iters, &next_move);
        double eval3 = eval1;//(double)negamax::predict(board, 3, eval_table, pred_table, &cumul_iters, &next_move);
        double eval_loss = get_eval_error(eval0, eval1, eval2, eval3);

        neg_evals += eval2 < LOSS/10;
        pos_evals += eval2 > WIN/10;

        // get eval gradients
        for (int i = 0; i < EVAL_TABLE_SIZE; i++)
        {
            (*eval_table)[i]++;
            double e0 = (double)board.evaluate(eval_table);
            double e1 = (double)negamax::predict(board, 1, eval_table, pred_table, &cumul_iters, &next_move);
            double e2 = (double)negamax::predict(board, 2, eval_table, pred_table, &cumul_iters, &next_move);
            double e3 = e1;//(double)negamax::predict(board, 3, eval_table, pred_table, &cumul_iters, &next_move);
            (*eval_table)[i]--;

            double new_loss = get_eval_error(e0, e1, e2, e3);
            eval_opt.grad[i] += eval_loss - new_loss;
        }

        // get prediction loss
        double pred_loss = get_pred_error(&board, eval_table, pred_table);

        // get pred gradients
        for (int i = 0; i < EVAL_TABLE_SIZE; i++)
        {
            (*pred_table)[i]++;
            double new_loss = get_pred_error(&board, eval_table, pred_table);
            (*pred_table)[i]--;

            pred_opt.grad[i] += pred_loss - new_loss;
        }

        // update stats
        cumul_eval_error += eval_loss;
        cumul_pred_error += pred_loss;
        // print summary
        if (iter % (BATCH_SIZE * PRINT_EVERY) == 0)
        {
#define PRINT_ARR(arr)                        \
    for (int i = 0; i < EVAL_TABLE_SIZE; i++) \
    {                                         \
        cout << arr[i] << ", ";               \
    }                                         \
    cout << "\n";

            cout << "Iteration: " << iter;
            cout << ": eval_loss " << (cumul_eval_error / (BATCH_SIZE * PRINT_EVERY));
            cout << ", pred_loss " << (cumul_pred_error / (BATCH_SIZE * PRINT_EVERY));
            cout << ", negamax_iters " << (cumul_iters / (BATCH_SIZE * PRINT_EVERY));
            cout << ", W/D/L " << wins << "/" << draws << "/" << losses;
            cout << ", LR " << LEARNING_RATE;
            cout << ", pos/neg evals " << pos_evals << "/" << neg_evals << "\n";

            cout << "Eval table: ";
            PRINT_ARR(eval_opt.vals);

            cout << "Pred table: ";
            PRINT_ARR(pred_opt.vals);
            cout << endl;

            //board.print();

            cumul_eval_error = 0;
            cumul_pred_error = 0;
            cumul_iters = 0;
            pos_evals = 0;
            neg_evals = 0;
            wins = 0;
            draws = 0;
            losses = 0;
        }

        // update values when batch is done
        if (iter % BATCH_SIZE == 0)
        {
            pred_opt.apply_gradient(LEARNING_RATE);
            eval_opt.apply_gradient(LEARNING_RATE);

            // make sure wins stay as wins
            eval_opt.vals[5] = WIN;
            eval_opt.vals[11] = LOSS;
            eval_opt.vals_d[5] = (double)WIN;
            eval_opt.vals_d[11] = (double)LOSS;
        }

        // randomly change move
        if (rand() % 20 == 0)
        {
            auto moves = board.get_moves(pred_table);
            next_move = moves[rand() % moves.size()].point;
        }

        // update board
        board.play(next_move);
        board_moves++;
        if (board.check_win(next_move) || board_moves > (BOARD_SIZE * BOARD_SIZE / 2))
        {
            if (board.check_win(next_move))
            {
                if (board.get_player())
                {
                    wins++;
                }
                else
                {
                    losses++;
                }
            }
            else
            {
                draws++;
            }
            board = NEW_BOARD();
            board_moves = 0;
        }

        // board.debug();
    }
    return 0;
};