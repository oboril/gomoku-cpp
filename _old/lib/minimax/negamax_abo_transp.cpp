#include "minimax.hpp"
#include "board.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>

struct Node
{
    bool initialized;
    int64_t alpha;
    int64_t beta;
    std::vector<Move> moves;
};

using TranspTable = std::unordered_map<Board, Node>;

int64_t _negamax_abo_transp(Board *board, int64_t depth, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters);

int64_t negamax_abo_transp(Board b, int64_t depth, int64_t *iters)
{
    TranspTable transp_table;
    return _negamax_abo_transp(&b, depth, LOSS, WIN, transp_table, iters);
}

int64_t _negamax_abo_transp(Board *board, int64_t depth, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters)
{
    (*iters)++;

    Node *cached = &transp_table[(*board)];

    // If node is cached, update alpha and beta
    if (cached->initialized)
    {
        alpha = MAX(alpha, cached->alpha);
        beta = MIN(beta, cached->beta);

        if (alpha >= beta)
        {
            return alpha;
        }
    }

    // Evaluate leaf nodes, cache evaluation
    if (depth == 0)
    {
        int64_t score = board->evaluate(&DEFAULT_EVAL_TABLE);
        cached->alpha = score;
        cached->beta = score;
        cached->initialized = true;
        return score;
    }

    // Generate possible moves if not cached
    if (!cached->initialized)
    {
        cached->moves = board->get_moves(&DEFAULT_EVAL_TABLE);
        // sort the moves
        std::sort(cached->moves.begin(), cached->moves.end(), [](const Move &lhs, const Move &rhs)
                  { return lhs.score > rhs.score; });

        cached->alpha = LOSS;
        cached->beta = WIN;
        cached->initialized = true;
    }

    // If no more moves are possible, it is a draw
    if (cached->moves.size() == 0)
    {
        cached->alpha = 0;
        cached->beta = 0;
        return 0;
    }

    int64_t best = LOSS;
    const int64_t new_alpha = -(beta + SIGN(beta));
    for (const Move m : cached->moves)
    {
        const Point p = m.point;
        board->play(p);

        // Check if the move is winning
        if (board->check_win(p))
        {
            board->reset_move(p);
            cached->alpha = WIN;
            cached->beta = WIN;
            return WIN;
        }

        // checkmate distance pruning
        if (best >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        int64_t score = _negamax_abo_transp(board, depth - 1, new_alpha, -(best + SIGN(best)), transp_table, iters);
        best = MAX(best, -score + SIGN(score));
        board->reset_move(p);

        // fail-high check
        if (best >= beta)
        {
            cached->alpha = best;
            return best;
        }
    }

    // if this is PV-node, store evaluation
    if (best > alpha && best < beta)
    {
        cached->alpha = best;
        cached->beta = best;
    }

    // fail low nodes
    if (best < alpha)
    {
        cached->beta = best;
    }

    return best;
}