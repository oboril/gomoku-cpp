#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>

int64_t _pvs_abo_fp(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters);

int64_t negamax::pvs_abo_fp(Board b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters)
{
    return _pvs_abo_fp(&b, depth, eval_table, predict_table, LOSS, WIN, iters);
}

int64_t _pvs_abo_fp(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, int64_t *iters)
{
    (*iters)++;

    if (depth == 0)
    {
        return board->evaluate(eval_table);
    }

    std::vector<Move> moves = board->get_moves(predict_table);

    if (moves.size() == 0)
    {
        return 0;
    }

    bool forced_move = (moves[0].score - moves[moves.size() - 1].score) > FORCING;

#ifdef NULL_MOVE_PRUNING
    if (depth <= NULL_MOVE_PRUNING && beta < WIN / 10)
    {
        board->swap_player();
        int64_t null_move_eval = -board->evaluate(eval_table);
        board->swap_player();
        if (null_move_eval > beta)
        {
            return null_move_eval;
        }
    }
#endif

#ifdef FUTILITY_PRUNING
    int64_t best_worst_move = moves[0].score - moves[moves.size() - 1].score;
#endif

    int64_t best = LOSS;
    int visited_children = 0;
    const int64_t new_alpha = -(beta + SIGN(beta));
    for (const Move m : moves)
    {
        visited_children++;
        const Point p = m.point;

        // check win
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

        // PVS
        const int64_t new_beta = -(best + SIGN(best));
        if (visited_children == 1)
        {
            // first search full window
            int64_t score = _pvs_abo_fp(board, depth - 1, eval_table, predict_table, new_alpha, new_beta, iters);
            best = MAX(best, -score + SIGN(score));
            board->reset_move(p);
        }
        else 
        {
            // try null window
            int64_t score = _pvs_abo_fp(board, depth - 1, eval_table, predict_table, new_beta-1, new_beta, iters);
            score = -score + SIGN(score);
            // if null window fails, try full window again
            if (score > best)
            {
                score = _pvs_abo_fp(board, depth - 1, eval_table, predict_table, new_alpha, new_beta, iters);
                best = MAX(best, -score + SIGN(score));
            }
            board->reset_move(p);
        }

        // if the first move is forced, don't try any more moves
        if (forced_move)
        {
            break;
        }

        if (best >= beta)
        {
            return best;
        }

#ifdef FUTILITY_PRUNING
        if (depth <= FUTILITY_PRUNING && visited_children == 1 && best < (alpha - best_worst_move))
        {
            return best;
        }
#endif

#ifdef LIMIT_MOVES
        if (visited_children >= LIMIT_MOVES(depth))
        {
            break;
        }
#endif
    }

    return best;
}