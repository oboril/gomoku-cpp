#include "board.hpp"
#include "minimax.hpp"
#include <iostream>
#include <chrono>
#include "adam.hpp"
#include "error_transform.hpp"
#include <algorithm>
using std::cout;
using std::endl;

#define DEBUG(x) cout << "DEBUG " << x << endl;

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
    //return std::pow(e1 - e3, 2) + std::pow(e2 - e4, 2);
}

std::vector<Move> get_best_moves(Board *board, const EvaluationTable *eval_table, const PredictionTable *pred_table)
{
    auto moves = board->get_moves(pred_table);
    std::vector<Move> output;
    for (Move m : moves)
    {
        board->play(m.point);
        double new_eval = -(double)board->evaluate(eval_table);
        board->reset_move(m.point);
        output.push_back(Move(m.point, new_eval));
    }
    std::sort(output.begin(), output.end(), [](const Move &lhs, const Move &rhs)
              { return lhs.score > rhs.score; });

    return output;
}

double get_pred_error(Board *board, std::vector<Move> &moves, int64_t curr_eval, const PredictionTable *pred_table)
{
    // best move should leave eval unchanged
    // predicted = current + move_prediction

    double error = 0.;

    auto pred_moves = board->get_moves(pred_table);

    // sum all errors
    for (const Move m : moves)
    {
        for (const Move pm : pred_moves)
        {
            if (m.point == pm.point)
            {
                double real_change = transform_value(m.score - curr_eval);
                double pred_change = transform_value(pm.score);

                error += std::pow(real_change - pred_change, 2);
                break;
            }
        }
    }

    return error / ((double)moves.size());
}

int main()
{
    // Seed random with time
    srand((unsigned int)time(NULL));

    //constexpr EvaluationTable eval_init = {
    //    0, 1000, 10000, 100000, 10000000, WIN,      // P1 counts
    //    10000, -500, -5000, -50000, -1000000, LOSS, // bias + P2 counts
    //    1000, 1000, 1000, 10000,                    // P1 11 22 33 44
    //    -1000, -1000, -1000, -10000,                // P2 11 22 33 44
    //    1000, 1000, 1000, 1000, 1000, 10000,        // P1 21 31 32 41 42 43
    //    -1000, -1000, -1000, -1000, -1000, -10000   // P2 21 31 32 41 42 43
    //};
    constexpr EvaluationTable eval_init = {8, 3377, 12540, 42779, 218406, 1000000000000, 14, -2098, -8800, -28593, -170960891, -1000000000000, 63, 15, -9, 15, -8277, -10909, -167238, -2594056, 252, 26, 9, 4, -10, 15, -9561, -7214, -19951, -204788, -242003, -2593398};
    constexpr PredictionTable pred_init = {568, 5780, 16914, 57450, FORCING * 100, WIN, -360, 1897, 9934, 33104, FORCING * 10, LOSS};

    // #define RN (rand()%10000-5000)
    // const EvaluationTable eval_init = {RN, RN, RN, RN, RN, WIN, RN, RN, RN, RN, RN, LOSS};
    // const EvaluationTable pred_init = {RN, RN, RN, RN, RN, WIN, RN, RN, RN, RN, RN, LOSS};
    // #undef RN

    constexpr double EVAL_LR = 0.03;
    constexpr double PRED_LR = 0.00;
    constexpr int BATCH_SIZE = 500;
    constexpr double NORM_CONST = 0.001; // prevents eval_table from going to 0
    constexpr int PRINT_EVERY = 10;
    constexpr int MAX_MOVES = 1; // 20; not learning right now
#define NEW_BOARD() Board::random(3)

    AdamOpt<32> eval_opt((int64_t *)&eval_init);
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
    double eval_lr;
    double pred_lr;
    Point next_move;

    for (int iter = 0; iter >= 0; iter++)
    {
        // adjust learning rate
        if (iter < BATCH_SIZE * 10)
        {
            // adjust optimizer hyperparameters
            eval_lr = 0;
            pred_lr = 0;
        }
        else if (iter < 300000)
        {
            eval_lr = EVAL_LR * 1.;
            pred_lr = PRED_LR * 1.;
        }
        else if (iter < 1000000)
        {
            eval_lr = EVAL_LR * 0.3;
            pred_lr = PRED_LR * 0.3;
        }
        else
        {
            eval_lr = EVAL_LR * 0.1;
            pred_lr = PRED_LR * 0.1;
        }

        // get board evaluation
        EvaluationTable *eval_table = (EvaluationTable *)&eval_opt.vals;
        PredictionTable *pred_table = (PredictionTable *)&pred_opt.vals;
        double eval0 = (double)board.evaluate(eval_table);
        negamax::Result eval1 = negamax::predict(board, 1, eval_table, pred_table, &cumul_iters);
        negamax::Result eval2 = negamax::predict(board, 2, eval_table, pred_table, &cumul_iters); // !!!!!!!!!
        negamax::Result eval3 = negamax::predict(board, 3, eval_table, pred_table, &cumul_iters); // !!!!!!!!!
        double eval_loss = get_eval_error(eval0, eval1.score, eval2.score, eval3.score);

        next_move = eval2.best_move;

        neg_evals += eval2.score < LOSS / 10;
        pos_evals += eval2.score > WIN / 10;

        // get eval gradients
        for (int i = 0; i < EVAL_TABLE_SIZE; i++)
        {
            (*eval_table)[i]++;
            double ev0 = (double)board.evaluate(eval_table);
            double ev1 = (double)eval1.final_board.evaluate(eval_table);
            double ev2 = (double)eval2.final_board.evaluate(eval_table); // !!!!!!!!!!
            double ev3 = (double)eval3.final_board.evaluate(eval_table); // !!!!!!!!!!
            (*eval_table)[i]--;
            if (eval1.final_board.get_player() != board.get_player())
            {
                ev1 *= -1.;
            }
            if (eval2.final_board.get_player() != board.get_player())
            {
                ev2 *= -1.;
            }
            if (eval3.final_board.get_player() != board.get_player())
            {
                ev3 *= -1.;
            }

            double new_loss = get_eval_error(ev0, ev1, ev2, ev3);

            // cout << "e0 " << eval0 << "/" << ev0 << " e2 " << eval2.score << "/" << ev2 << endl;

            eval_opt.grad[i] += eval_loss - new_loss;
        }

        // get prediction loss
        auto moves = get_best_moves(&board, eval_table, pred_table);
        while (moves.size() > MAX_MOVES)
        {
            moves.pop_back();
        }
        double pred_loss = get_pred_error(&board, moves, eval0, pred_table);

        // get pred gradients
        for (int i = 0; i < PRED_TABLE_SIZE; i++)
        {
            (*pred_table)[i]++;
            double new_loss = get_pred_error(&board, moves, eval0, pred_table);
            (*pred_table)[i]--;

            pred_opt.grad[i] += pred_loss - new_loss;
        }

        // update stats
        cumul_eval_error += eval_loss;
        cumul_pred_error += pred_loss;
        // print summary
        if (iter % (BATCH_SIZE * PRINT_EVERY) == 0)
        {
#define PRINT_ARR(arr, size)       \
    for (int i = 0; i < size; i++) \
    {                              \
        cout << arr[i] << ", ";    \
    }                              \
    cout << "\n";

            cout << "Iteration: " << iter;
            cout << ": eval_loss " << (cumul_eval_error / (BATCH_SIZE * PRINT_EVERY));
            cout << ", pred_loss " << (cumul_pred_error / (BATCH_SIZE * PRINT_EVERY));
            cout << ", negamax_iters " << (cumul_iters / (BATCH_SIZE * PRINT_EVERY));
            cout << ", W/D/L " << wins << "/" << draws << "/" << losses;
            cout << ", EVAL_LR " << eval_lr;
            cout << ", PRED_LR " << pred_lr;
            cout << ", pos/neg evals " << pos_evals << "/" << neg_evals << "\n";

            cout << "Eval table: ";
            PRINT_ARR(eval_opt.vals, EVAL_TABLE_SIZE);

            cout << "Pred table: ";
            PRINT_ARR(pred_opt.vals, PRED_TABLE_SIZE);
            cout << endl;

            // board.print();

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
            // for (int i = 0; i < EVAL_TABLE_SIZE; i++)
            for (int i = 0; i < 12; i++)
            {
                // NORMALIZTION LOSS: norm_loss = - norm_const * asinh(abs(weight))
                double norm_grad = 1 / std::sqrt(1 + std::pow(eval_opt.vals_d[i], 2)) * (double)SIGN(eval_opt.vals_d[i]);
                norm_grad *= NORM_CONST * BATCH_SIZE;
                eval_opt.grad[i] += norm_grad;
            }
            pred_opt.apply_gradient(pred_lr);
            eval_opt.apply_gradient(eval_lr);

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