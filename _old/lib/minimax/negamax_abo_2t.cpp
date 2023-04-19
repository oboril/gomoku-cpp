#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

int64_t _negamax_abo_2t(Board *board, int64_t depth, int64_t alpha, int64_t beta, int64_t *iters);

int64_t negamax_abo_2t(Board b, int64_t depth, int64_t *iters)
{
    return _negamax_abo_2t(&b, depth, LOSS, WIN, iters);
}

int64_t _negamax_abo_2t(Board *board, int64_t depth, int64_t alpha, int64_t beta, int64_t *iters)
{
    (*iters)++;

    if (depth == 0)
    {
        return board->evaluate(&DEFAULT_EVAL_TABLE);
    }

    std::vector<Move> moves = board->get_moves(&DEFAULT_PREDICT_TABLE);
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

        int64_t score = _negamax_abo_2t(board, depth - 1, -(beta + SIGN(beta)), -(best + SIGN(best)), iters);
        best = MAX(best, -score + SIGN(score));
        board->reset_move(p);

        if (best >= beta)
        {
            return best;
        }
    }

    return best;
}