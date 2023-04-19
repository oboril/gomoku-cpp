#include "board.hpp"
#include <cstring>

Board::Board() {
    memset(this, 0, sizeof(Board));
    
    //for (size_t i = 0; i < BOARD_SIZE; i++) {
    //    m_board[0][i] = 0;
    //    m_board[1][i] = 0;
    //}
    m_player = true;
}

Board::Board(const Board & board) {
    memcpy(this, &board, sizeof(Board));
    //for (size_t i = 0; i < BOARD_SIZE; i++) {
    //    m_board[0][i] = board.m_board[0][i];
    //    m_board[1][i] = board.m_board[1][i];
    //}
    //m_player = board.m_player;
}

void Board::set(Point p, bool player) {
    m_board[player][p.y] |= 1<<p.x;
}

void Board::swap_player() {
    m_player = !m_player;
}

void Board::play(Point p) {
    set(p, m_player);
    swap_player();
}

void Board::reset_move(Point p) {
    swap_player();
    m_board[m_player][p.y] &= ~(1<<p.x);
}

// returns false if the move is illegal
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

bool Board::is_empty(Point p) const {
    return !(((m_board[0][p.y] >> p.x) | (m_board[1][p.y] >> p.x)) & 1);
}

