#include "minimax.hpp"
#include "board.hpp"

int64_t _negamax_alphabeta(Board * b, int64_t depth, int64_t alpha, int64_t beta, int64_t * iters);

int64_t negamax_alphabeta(Board b, int64_t depth, int64_t * iters) {
    return _negamax_alphabeta(&b, depth, LOSS, WIN, iters);
}

int64_t _negamax_alphabeta(Board * b, int64_t depth, int64_t alpha, int64_t beta, int64_t * iters) {
    (*iters)++;

    if (depth == 0) {
        return b->evaluate(&DEFAULT_EVAL_TABLE);
    }

    int64_t best = LOSS;
    bool any_empty = false;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            const Point p(x,y);
            if (!b->is_empty(p)) {
                continue;
            }

            any_empty = true;

            b->play(p);
            if (b->check_win(p)) {
                b->reset_move(p);
                return WIN;
            }

            int64_t score = _negamax_alphabeta(b, depth-1, -(beta + SIGN(beta)), -(best + SIGN(best)), iters);
            best = MAX(best, -score + SIGN(score));
            b->reset_move(p);

            if (best >= beta) {
                return best;
            }
        }
    }

    if (!any_empty) {
        return 0;
    }

    return best;
}