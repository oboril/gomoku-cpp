#include "board.hpp"
#include <cstring>

bool Board::operator==(const Board & rhs) const {
    return  memcmp(this, &rhs, sizeof(Board)) == 0;
}

size_t Board::hash() const {
    size_t hash = 0xc676f3e754785dd5;
    constexpr int iters = sizeof(m_board) / sizeof(size_t);
    size_t * vals = (size_t*) m_board;
    for (int i = 0; i < iters; i++) {
        hash ^= vals[i];
        hash = _rotl64(hash, 31) ^ hash * 0x3c2f6853e7989351;
    }
    return hash ^ m_player;
}