#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

int64_t _abo(Board *board, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t alpha, int64_t beta, int64_t *iters);

int64_t negamax::abo(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t *iters)
{
    return _abo(&b, depth, eval_table, predict_table, LOSS, WIN, iters);
}

int64_t _abo(Board *board, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t alpha, int64_t beta, int64_t *iters)
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

    bool forced_move = (moves[0].score - moves[moves.size() - 1].score) > FORCING;

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
        if (best >= WIN - 2 || alpha >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        int64_t score = _abo(board, depth - 1, eval_table, predict_table, -(beta + SIGN(beta)), -(best + SIGN(best)), iters);
        best = MAX(best, -score + SIGN(score));
        board->reset_move(p);

        // if the first move is forced, don't try any more moves
        if (forced_move)
        {
            break;
        }

        if (best >= beta)
        {
            return best;
        }
    }

    return best;
}