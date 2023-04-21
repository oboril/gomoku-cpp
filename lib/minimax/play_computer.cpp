#include "minimax.hpp"
#include "board.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <iostream>

using namespace negamax;

int64_t _play_computer(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters, Point * best_move);

int64_t negamax::play_computer(Board b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, TranspTable &transp_table, int64_t *iters, Point * best_move)
{
    for (int64_t id = 1; id < depth; id++)
    {
        _play_computer(&b, id, eval_table, predict_table, LOSS, WIN, transp_table, iters, best_move);
    }

    return _play_computer(&b, depth, eval_table, predict_table, LOSS, WIN, transp_table, iters, best_move);
}

int64_t _play_computer(Board *board, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters, Point * best_move)
{
    (*iters)++;

    NodeItDFp *cached = &transp_table[(*board)];

    // If node is cached, update alpha and beta
    if (cached->depth >= depth)
    {
        alpha = MAX(alpha, cached->alpha);
        beta = MIN(beta, cached->beta);

        if (alpha >= beta)
        {
            return alpha;
        }
    }

    // LEAF NODE, evaluate, cache, and return
    if (depth == 0)
    {
        int64_t score = board->evaluate(eval_table);
        cached->alpha = score;
        cached->beta = score;
        cached->depth = 0;
        return score;
    }

    // NULL MOVE PRUNING
#ifdef NULL_MOVE_PRUNING
    if (depth <= NULL_MOVE_PRUNING && beta < WIN / 10)
    {
        if (cached->null_move_eval == NOT_CACHED)
        {
            board->swap_player();
            cached->null_move_eval = -board->evaluate(eval_table);
            board->swap_player();
        }
        if (cached->null_move_eval > beta)
        {
            return cached->null_move_eval;
        }
    }
#endif

    // Generate possible moves if not cached
    if (cached->depth < 1)
    {
        cached->moves = board->get_moves(predict_table);

#ifdef FUTILITY_PRUNING
        if (cached->moves.size() > 0)
        {
            cached->best_worst_move = cached->moves[0].score - cached->moves[cached->moves.size() - 1].score;
        }
#endif

        // if the move is forced, drop all other moves
        if (cached->moves.size() > 1 && cached->moves[cached->moves.size() - 1].score < -FORCING)
        {
            cached->moves.resize(1);
        }
    }

    // If no more moves are possible, it is a draw
    if (cached->moves.size() == 0)
    {
        cached->alpha = 0;
        cached->beta = 0;
        depth = TERMINAL;
        return 0;
    }

    // reset alpha and beta if higher depth is needed
    if (cached->depth < depth)
    {
        cached->alpha = LOSS;
        cached->beta = WIN;
        cached->depth = depth;
    }

    int64_t best = LOSS;
    const int64_t new_alpha = -(beta + SIGN(beta));
    int64_t visited_children = 0;
    for (Move &m : cached->moves)
    {
        visited_children++;

        const Point p = m.point;
        board->play(p);

        // Check if the move is winning
        if (board->check_win(p))
        {
            board->reset_move(p);
            cached->alpha = WIN;
            cached->beta = WIN;
            cached->depth = TERMINAL;
            m.score = WIN;
            best = WIN;
            (*best_move) = p;
            break;
        }

        // checkmate distance pruning
        if (best >= WIN - 2 || alpha >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        const int64_t new_beta = -(best + SIGN(best));
        // PVS
        if (visited_children == 1)
        {
            // search first child with full window
            int64_t score = _play_computer(board, depth - 1, eval_table, predict_table, new_alpha, new_beta, transp_table, iters, best_move);
            score = -score + SIGN(score);
            m.score = score;
            best = MAX(best, score);
            board->reset_move(p);

            (*best_move) = p;
        }
        else
        {
            // try null window search
            int64_t score = _play_computer(board, depth - 1, eval_table, predict_table, new_beta-1, new_beta, transp_table, iters, best_move);
            score = -score + SIGN(score);
            // if null window failed, redo the search
            if (score > best)
            {
                score = _play_computer(board, depth - 1, eval_table, predict_table, new_alpha, new_beta, transp_table, iters, best_move);
                score = -score + SIGN(score);

                (*best_move) = p;
            }
            m.score = score;
            best = MAX(best, score);
            board->reset_move(p);
        }

        // fail-high check
        if (best >= beta)
        {
            cached->alpha = best;
            break;
        }

#ifdef FUTILITY_PRUNING
        if (depth <= FUTILITY_PRUNING && visited_children == 1 && best < (alpha - cached->best_worst_move))
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

    // if this is PV-node, store evaluation
    if (best > alpha && best < beta)
    {
        cached->alpha = best;
        cached->beta = best;
    }

    // ALL-NODE, fail low node
    if (best <= alpha)
    {
        cached->beta = best;
    }

    // Sort visited children
    std::stable_sort(&cached->moves[0], &cached->moves[visited_children], [](const Move &lhs, const Move &rhs)
                     { return lhs.score > rhs.score; });

    return best;
}