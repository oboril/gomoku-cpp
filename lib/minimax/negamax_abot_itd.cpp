#include "minimax.hpp"
#include "board.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>

#define TERMINAL 999999

struct NodeItD
{
    int64_t depth;
    int64_t alpha;
    int64_t beta;
    std::vector<Move> moves;
    NodeItD() : depth(-1), alpha(LOSS), beta(WIN) {}
};

using TranspTable = std::unordered_map<Board, NodeItD>;

int64_t _negamax_abot_itd(Board *board, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters);

int64_t negamax_abot_itd(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t *iters)
{
    TranspTable transp_table;

    for (int64_t id = 1; id < depth; id++)
    {
        _negamax_abot_itd(&b, id, eval_table, predict_table, LOSS, WIN, transp_table, iters);
    }

    return _negamax_abot_itd(&b, depth, eval_table, predict_table, LOSS, WIN, transp_table, iters);
}

int64_t _negamax_abot_itd(Board *board, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t alpha, int64_t beta, TranspTable &transp_table, int64_t *iters)
{
    (*iters)++;

    NodeItD *cached = &transp_table[(*board)];

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

    // Evaluate leaf nodes, cache evaluation
    if (depth == 0)
    {
        int64_t score = board->evaluate(&DEFAULT_EVAL_TABLE);
        cached->alpha = score;
        cached->beta = score;
        cached->depth = 0;
        return score;
    }

    // Generate possible moves if not cached
    if (cached->depth < 1)
    {
        cached->moves = board->get_moves(&DEFAULT_EVAL_TABLE);

        // TODO
        // adjust the move score to board evaluation

        // find best child

        // eval best child

        // adjust evaluation of all the other possible moves

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

    // sort the moves by better evaluation
    std::sort(cached->moves.begin(), cached->moves.end(), [](const Move &lhs, const Move &rhs)
              { return lhs.score > rhs.score; });

    int64_t best = LOSS;
    const int64_t new_alpha = -(beta + SIGN(beta));
    for (Move& m : cached->moves)
    {
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
            return WIN;
        }

        // checkmate distance pruning
        if (best >= WIN - 2)
        {
            board->reset_move(p);
            continue;
        }

        int64_t score = _negamax_abot_itd(board, depth - 1, eval_table, predict_table, new_alpha, -(best + SIGN(best)), transp_table, iters);
        score = -score + SIGN(score);
        m.score = score;
        best = MAX(best, score);
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