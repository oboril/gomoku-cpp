#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

int64_t _negamax_ab_ordered(Board *board, int64_t depth, EvaluationTable * eval_table, int64_t alpha, int64_t beta, int64_t *iters);

int64_t negamax_ab_ordered(Board b, int64_t depth, EvaluationTable * eval_table, int64_t *iters)
{
    return _negamax_ab_ordered(&b, depth, eval_table, LOSS, WIN, iters);
}

int64_t _negamax_ab_ordered(Board *board, int64_t depth, EvaluationTable * eval_table, int64_t alpha, int64_t beta, int64_t *iters)
{
    (*iters)++;

    if (depth == 0)
    {
        return board->evaluate(eval_table);
    }

    std::vector<Move> moves = board->get_moves(eval_table);
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
            return WIN;
        }

        // checkmate distance pruning
        if (best >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        int64_t score = _negamax_ab_ordered(board, depth - 1, eval_table, -(beta + SIGN(beta)), -(best + SIGN(best)), iters);
        best = MAX(best, -score + SIGN(score));
        board->reset_move(p);

        if (best >= beta)
        {
            return best;
        }
    }

    return best;
}