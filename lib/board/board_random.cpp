#include "board.hpp"

Board Board::random(int iters)
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