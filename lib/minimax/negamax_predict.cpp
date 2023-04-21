#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

using namespace negamax;

Result _negamax_predict(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters);

Result negamax::predict(Board b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters)
{
    Result res = _negamax_predict(&b, depth, eval_table, predict_table, LOSS, WIN, iters);
    res.score += depth * SIGN(res.score);
    return res;
}

Result _negamax_predict(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters)
{
    (*iters)++;

    if (depth == 0)
    {
        return Result(board->evaluate(eval_table), Point(), (*board));
    }

    std::vector<Move> moves = board->get_moves(predict_table);

    if (moves.size() == 0)
    {
        return Result(0, Point(), (*board));
    }

    int64_t best = LOSS;
    Board best_board;
    Point best_move;
    int visited_children = 0;
    for (const Move m : moves)
    {
        visited_children++;

        const Point p = m.point;

        board->play(p);
        if (board->check_win(p))
        {
            Board ret_board = (*board);
            board->reset_move(p);
            return Result(WIN, p, ret_board);
        }

        // checkmate distance pruning
        if (best >= WIN - 2 || alpha >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        Result eval = _negamax_predict(board, depth - 1, eval_table, predict_table, -(beta + SIGN(beta)), -(best + SIGN(best)), iters);
        int64_t score =  -eval.score + SIGN(eval.score);
        if (score > best)
        {
            best_move = p;
            best_board = eval.final_board;
            best = score;
        }
        board->reset_move(p);

        if (best >= beta)
        {
            break;
        }

#ifdef LIMIT_MOVES
        if (visited_children >= LIMIT_MOVES(depth))
        {
            break;
        }
#endif
    }

    return Result(best, best_move, best_board);
}