#pragma once

#include "board.hpp"

int64_t simple_negamax(Board b, int64_t depth, int64_t * iters);

int64_t negamax_alphabeta(Board b, int64_t depth, int64_t * iters);

int64_t negamax_ab_ordered(Board b, int64_t depth, EvaluationTable* eval_table, int64_t * iters);

int64_t negamax_abo_transp(Board b, int64_t depth, int64_t * iters);

int64_t negamax_abo_2t(Board b, int64_t depth, int64_t * iters);

int64_t negamax_abot_itd(Board b, int64_t depth, int64_t * iters);