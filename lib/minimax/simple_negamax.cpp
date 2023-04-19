#include "minimax.hpp"
#include "board.hpp"

int64_t _simple_negamax(Board *b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters);

int64_t simple_negamax(Board b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters)
{
    return _simple_negamax(&b, depth, eval_table, predict_table, iters);
}

int64_t _simple_negamax(Board *b, int64_t depth, const EvaluationTable *eval_table, const EvaluationTable *predict_table, int64_t *iters)
{
    (*iters)++;

    if (depth == 0)
    {
        return b->evaluate(eval_table);
    }

    int64_t best = LOSS;
    bool any_empty = false;
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        for (int y = 0; y < BOARD_SIZE; y++)
        {
            const Point p(x, y);
            if (!b->is_empty(p))
            {
                continue;
            }

            any_empty = true;

            b->play(p);
            if (b->check_win(p))
            {
                b->reset_move(p);
                return WIN;
            }

            int64_t score = _simple_negamax(b, depth - 1, eval_table, predict_table, iters);
            best = MAX(best, -score + SIGN(score));
            b->reset_move(p);
        }
    }

    if (!any_empty)
    {
        return 0;
    }

    return best;
}