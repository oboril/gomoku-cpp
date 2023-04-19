#include "board.hpp"
#include <chrono>

Board random_board(int iters)
{
    Board b;
    while (iters > 0)
    {
        int x = (std::rand() % 4) + (BOARD_SIZE / 2 - 2);
        int y = (std::rand() % 4) + (BOARD_SIZE / 2 - 2);
        if (b.play_checked(Point(x, y)))
        {
            iters--;
        }
    }
    return b;
}

bool play_move(Board *board, const EvaluationTable *eval_table)
{
    auto moves = board->get_moves(eval_table);
    Move best_move(Point(0, 0), LOSS);
    for (Move m : moves)
    {
        if (m.score > best_move.score)
        {
            best_move = m;
        }
    }
    board->play(best_move.point);
    return board->check_win(best_move.point);
}

int main() {
    // Seed random with time
    srand((unsigned int)time(NULL));

    Board b = random_board(4);
    b.debug();
    while (!play_move(&b, &DEFAULT_PREDICT_TABLE)) {
        b.debug();
    }
    b.debug();
}