#include "board.hpp"
#include <cstring>

Board::Board() {
    memset(this, 0, sizeof(Board));

    m_player = true;
}

Board::Board(const Board & board) {
    memcpy(this, &board, sizeof(Board));
}

// Sets a mark to the specified location
void Board::set(Point p, bool player) {
    m_board[player][p.y] |= 1<<p.x;
}

// Swaps players
void Board::swap_player() {
    m_player = !m_player;
}

// Places stone on the specified location and swaps players
void Board::play(Point p) {
    set(p, m_player);
    swap_player();
}

// Undoes Board::play
void Board::reset_move(Point p) {
    swap_player();
    m_board[m_player][p.y] &= ~(1<<p.x);
}

// Attempts to place stone, returns false if illegal
bool Board::play_checked(Point p)
{
    if (p.x < 0 || p.x >= BOARD_SIZE || p.y < 0 || p.y > BOARD_SIZE)
    {
        return false;
    }
    if (!is_empty(p))
    {
        return false;
    }
    play(p);
    return true;
}

// Returns true if the given square is empty
bool Board::is_empty(Point p) const {
    return !(((m_board[0][p.y] >> p.x) | (m_board[1][p.y] >> p.x)) & 1);
}

