#include "board.hpp"
#include <string>
#include <iostream>

#define MOVE if (!play_checked(Point(x, BOARD_SIZE - y))){return false;} y=0;x=0;

bool Board::moves_from_string(const std::string &moves)
{
    size_t x = 0;
    size_t y = 0;
    bool first = true;
    for (char ch : moves)
    {
        if (ch >= '0' && ch <= '9')
        {
            y = y * 10 + (ch - '0');
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            if (!first)
            {
                MOVE
            }
            x = ch - 'A';
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            if (!first)
            {
                MOVE
            }
            x = ch - 'a';
        }
        else
        {
            return false;
        }

        first = false;
    }

    MOVE

    return true;
}