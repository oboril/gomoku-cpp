#pragma once

#include "board.hpp"

namespace negamax
{
    int64_t abo(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);
    int64_t abot_itd(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);

    int64_t abo_fp(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);
    int64_t abot_itd_fp(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);

    int64_t pvs_abo_fp(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);
    int64_t pvs_abot_itd_fp(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters);

    int64_t predict(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters, Point * best_move);

    int64_t play_computer(Board b, int64_t depth, const EvaluationTable * eval_table, const EvaluationTable * predict_table, int64_t * iters, Point * best_move);

    // Maximum depth for null move pruning
    //#define NULL_MOVE_PRUNING 3
    // the evaluation time is more than what is saved

    // Maximum depth for futility pruning - no extra eval cost
    #define FUTILITY_PRUNING 2

    // Limit for how many moves to explore at given depth
    //#define LIMIT_MOVES(x) ((x)*5)
    #define LIMIT_MOVES(x) ((x)*5)
}