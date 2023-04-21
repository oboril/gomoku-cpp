#include "board.hpp"
#include "colors.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

#define LARGE_BOARD 8

void Board::print() const
{
    cout << "     ";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << " " << (char)(i + 'A') << " ";
    }
    cout << "\n    -";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << "---";
    }
    cout << "-\n";

    for (size_t y = 0; y < BOARD_SIZE; y++)
    {
        cout << setfill(' ') << std::setw(3) << (BOARD_SIZE - y) << " |";
        for (size_t x = 0; x < BOARD_SIZE; x++)
        {
            uint16_t b1 = (m_board[0][y] >> x) & 1;
            uint16_t b2 = (m_board[1][y] >> x) & 1;
            if (b1 && b2)
            {
                cout << BOLDGREEN << " ? " << RESET;
            }
            else if (b1)
            {
                cout << BOLDCYAN << " O " << RESET;
            }
            else if (b2)
            {
                cout << BOLDRED << " X " << RESET;
            }
            else
            {
                cout << " . ";
            }
        }
#if BOARD_SIZE > LARGE_BOARD
        cout << "| " << (BOARD_SIZE - y) << "\n";
#else
        cout << "|\n";
#endif
    }

    cout << "    -";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << "---";
    }
    cout << "-\n";

#if BOARD_SIZE > LARGE_BOARD
    cout << "     ";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << " " << (char)(i + 'A') << " ";
    }
    cout << "\n";
#endif
}

void Board::debug() const
{
    print();
    while (cin.get() != '\n')
    {
    }
}

void Board::print_highlight(Point p) const
{
    cout << "     ";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << " " << (char)(i + 'A') << " ";
    }
    cout << "\n    -";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << "---";
    }
    cout << "-\n";

    for (size_t y = 0; y < BOARD_SIZE; y++)
    {
        cout << setfill(' ') << std::setw(3) << (BOARD_SIZE - y) << " |";
        for (size_t x = 0; x < BOARD_SIZE; x++)
        {
            uint16_t b1 = (m_board[0][y] >> x) & 1;
            uint16_t b2 = (m_board[1][y] >> x) & 1;
            if (p.x == x && p.y == y)
            {
                if (b1 && b2)
                {
                    cout << BOLDGREEN << ONGRAY << " ? " << RESET;
                }
                else if (b1)
                {
                    cout << BOLDCYAN << ONGRAY << " O " << RESET;
                }
                else if (b2)
                {
                    cout << BOLDRED << ONGRAY << " X " << RESET;
                }
                else
                {
                    cout  << ONGRAY << " . " << RESET;
                }
            }
            else
            {
                if (b1 && b2)
                {
                    cout << BOLDGREEN << " ? " << RESET;
                }
                else if (b1)
                {
                    cout << BOLDCYAN << " O " << RESET;
                }
                else if (b2)
                {
                    cout << BOLDRED << " X " << RESET;
                }
                else
                {
                    cout << " . ";
                }
            }
        }
#if BOARD_SIZE > LARGE_BOARD
        cout << "| " << (BOARD_SIZE - y) << "\n";
#else
        cout << "|\n";
#endif
    }

    cout << "    -";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << "---";
    }
    cout << "-\n";

#if BOARD_SIZE > LARGE_BOARD
    cout << "     ";
    for (char i = 0; i < BOARD_SIZE; i++)
    {
        cout << " " << (char)(i + 'A') << " ";
    }
    cout << "\n";
#endif
}