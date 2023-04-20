#include <iostream>
#include "board.hpp"
#include "minimax.hpp"
#include "timer.hpp"

void bench_board(Board b);

int main() {
    Board b;
    b.moves_from_string("c3e5c4e4d2b4e2a3");
    bench_board(b);

    b = Board();
    b.moves_from_string("c3d4b4d2");
    bench_board(b);

    b = Board();
    b.moves_from_string("c4a1c3e3c2d5");
    //bench_board(b);

    return 0;
}

#define BENCH(func, depth, name) iters = 0; { Timer _t; eval = func(b, depth, &DEFAULT_EVAL_TABLE, &DEFAULT_PREDICT_TABLE, &iters);} std::cout << name << " evaluation (d=" << depth << "): " << eval << " (" << iters << " iters)" << std::endl;

void bench_board(Board b) {
    b.print();
    int64_t iters = 0;

    // Only eval table
    int64_t eval = b.evaluate(&DEFAULT_EVAL_TABLE);
    std::cout << "Evaluation (depth 0): " << eval << "\n";
    b.swap_player();
    const int64_t passing_move = -b.evaluate(&DEFAULT_EVAL_TABLE);
    b.swap_player();
    std::cout << "Passing move score: " << passing_move << "\n" << std::endl;

    //BENCH(simple_negamax, 1,     "Negamax        ")
    //BENCH(simple_negamax, 2,     "Negamax        ")
    //BENCH(simple_negamax, 3,     "Negamax        ")
    //BENCH(simple_negamax, 4,     "Negamax        ")
    //BENCH(negamax_alphabeta, 2,  "Negamax a-b    ")
    //BENCH(negamax_alphabeta, 3,  "Negamax a-b    ")
    //BENCH(negamax_alphabeta, 5,  "Negamax a-b    ")
    //BENCH(negamax_ab_ordered, 4, "Negamax a-b+o  ")
    //BENCH(negamax_ab_ordered, 3, "Negamax a-b+o  ")
    //BENCH(negamax_ab_ordered, 4, "Negamax a-b+o  ")
    //BENCH(negamax_ab_ordered, 3, "Negamax abo    ")
    //BENCH(negamax_ab_ordered, 4, "Negamax abo    ")
    //BENCH(negamax_ab_ordered, 5, "Negamax abo    ")
    //BENCH(negamax_abo_transp, 3, "Negamax a-b+o+t")
    //BENCH(negamax_abo_transp, 4, "Negamax a-b+o+t")

    //BENCH(negamax_abot_itd, 3,   "Negamax abot+id")
    //BENCH(negamax_abot_itd, 4,   "Negamax abot+id")
    //BENCH(negamax_abot_itd, 5,   "Negamax abot+id")

    BENCH(negamax_ab_ordered, 4, "Negamax abo    ")
    BENCH(negamax_abot_itd, 4,   "Negamax abot+id")
    BENCH(negamax_ab_ordered, 5, "Negamax abo    ")
    BENCH(negamax_abot_itd, 5,   "Negamax abot+id")
    BENCH(negamax_ab_ordered, 6, "Negamax abo    ")
    BENCH(negamax_abot_itd, 6,   "Negamax abot+id")
}