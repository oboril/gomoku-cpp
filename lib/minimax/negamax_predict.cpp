#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

int64_t _negamax_predict(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters, Point *best_move);

int64_t negamax_predict(Board b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters, Point *best_move)
{
    return _negamax_predict(&b, depth, eval_table, predict_table, LOSS, WIN, iters, best_move);
}

int64_t _negamax_predict(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters, Point *best_move)
{
    (*iters)++;

    if (depth == 0)
    {
        return board->evaluate(eval_table);
    }

    std::vector<Move> moves = board->get_moves(predict_table);
    // sort the moves
    std::sort(moves.begin(), moves.end(), [](const Move &lhs, const Move &rhs)
              { return lhs.score > rhs.score; });

    if (moves.size() == 0)
    {
        return 0;
    }

    int64_t best = LOSS;
    for (const Move m : moves)
    {
        const Point p = m.point;
        if (!board->is_empty(p))
        {
            continue;
        }

        board->play(p);
        if (board->check_win(p))
        {
            board->reset_move(p);
            *best_move = m.point;
            return WIN;
        }

        // checkmate distance pruning
        if (best >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        int64_t score = _negamax_predict(board, depth - 1, eval_table, predict_table, -(beta + SIGN(beta)), -(best + SIGN(best)), iters, best_move);
        score = -score + SIGN(score);
        if (score > best)
        {
            *best_move = m.point;
        }
        best = MAX(best, score);
        board->reset_move(p);

        if (best >= beta)
        {
            return best;
        }
    }

    return best;
}