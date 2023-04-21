#pragma once

#include "board.hpp"
#include <unordered_map>

namespace negamax
{
        struct NodeItDFp;
        using TranspTable = std::unordered_map<Board, NodeItDFp>;
        struct Result;

        int64_t abo(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);
        int64_t abot_itd(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);

        int64_t abo_fp(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);
        int64_t abot_itd_fp(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);

        int64_t pvs_abo_fp(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);
        int64_t pvs_abot_itd_fp(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);

        Result predict(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, int64_t *iters);

        int64_t play_computer(Board b, int64_t depth, const EvaluationTable *eval_table, const PredictionTable *predict_table, TranspTable &transp_table, int64_t *iters, Point *best_move);

// Maximum depth for null move pruning
// #define NULL_MOVE_PRUNING 3
// the evaluation time is more than what is saved

// Maximum depth for futility pruning - no extra eval cost
#define FUTILITY_PRUNING 2

// Limit for how many moves to explore at given depth
// #define LIMIT_MOVES(x) ((x)*5)
#define LIMIT_MOVES(x) ((x)*5)

#define TERMINAL 999999
#define NOT_CACHED (WIN * 10)

        struct Result
        {
                int64_t score;
                Point best_move;
                Board final_board;
                Result() : score(NOT_CACHED), best_move(Point(-1,-1)), final_board(Board()) {}
                Result(int64_t sc, Point move, Board board) : score(sc), best_move(move), final_board(board) {}
        };

        struct NodeItDFp
        {
                int64_t depth;
                int64_t alpha;
                int64_t beta;
                std::vector<Move> moves;

#ifdef NULL_MOVE_PRUNING
                int64_t null_move_eval;
#endif
#ifdef FUTILITY_PRUNING
                int64_t best_worst_move;
#endif
                NodeItDFp() : depth(-1), alpha(LOSS), beta(WIN)
                {
#ifdef NULL_MOVE_PRUNING
                        null_move_eval = NOT_CACHED;
#endif
#ifdef FUTILITY_PRUNING
                        best_worst_move = NOT_CACHED;
#endif
                }
        };
}