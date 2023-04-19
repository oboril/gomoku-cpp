#include "board.hpp"

// Checks if the last move resulted in a win
bool Board::check_win(Point p) const
{
    const BoardRow *board = m_board[!m_player];

    // ROW
    for (int x = MAX(p.x - 4, 0); x < MIN(p.x + 1, BOARD_SIZE - 4); x++)
    {
        if (((board[p.y] >> x) & 0b11111) == 0b11111)
        {
            return true;
        }
    }

    // COLUMNS
    const uint16_t mask = 1 << p.x;
    for (int y = MAX(p.y - 4, 0); y < MIN(p.y + 1, BOARD_SIZE - 4); y++)
    {
        if (
            (board[y] & mask) &&
            (board[y + 1] & mask) &&
            (board[y + 2] & mask) &&
            (board[y + 3] & mask) &&
            (board[y + 4] & mask))
        {
            return true;
        }
    }

    // MAIN DIAGONAL
    const int min_offset = MAX(-MIN(p.x, p.y), -4);
    const int max_offset = MIN((BOARD_SIZE - MAX(p.x, p.y)) - 5, 0);
    for (int off = min_offset; off <= max_offset; off++)
    {
        int x = p.x + off;
        int y = p.y + off;
        if (
            (board[y] & (0b1 << x)) &&
            (board[y + 1] & (0b10 << x)) &&
            (board[y + 2] & (0b100 << x)) &&
            (board[y + 3] & (0b1000 << x)) &&
            (board[y + 4] & (0b10000 << x)))
        {
            return true;
        }
    }

    // OTHER DIAGONAL
    const int space_right = BOARD_SIZE - p.x - 1;
    const int space_bottom = BOARD_SIZE - p.y - 1;
    // offset = 0 corresponds to point in left bottom
    const int min_offset2 = MAX(-4, -MIN(p.x, space_bottom));
    const int max_offset2 = MIN(0, MIN(p.y, space_right) - 4);
    for (int off = min_offset2; off <= max_offset2; off++)
    {
        int x = p.x + off;
        int y = p.y - off;
        if (
            (board[y] & (0b1 << x)) &&
            (board[y - 1] & (0b10 << x)) &&
            (board[y - 2] & (0b100 << x)) &&
            (board[y - 3] & (0b1000 << x)) &&
            (board[y - 4] & (0b10000 << x)))
        {
            return true;
        }
    }

    return false;
}