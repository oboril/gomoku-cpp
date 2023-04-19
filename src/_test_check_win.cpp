#include <iostream>
#include "board.hpp"

// MADE FOR BOARD SIZE 6

void test(const std::string &init, Point p, bool expected)
{
    Board b;
    b.moves_from_string(init);
    if (b.check_win(p) != expected)
    {
        std::cout << "Unexpected board result, expected " << expected << std::endl;
        b.print();
    }
}

int main()
{
    // ROWS
    test("A1A5B1B5C1C5D1D5E1", Point(0, 5), true);
    test("A1A5B1B5C1C5D1D5E1", Point(1, 5), true);
    test("A1A5B1B5C1C5D1D5E1", Point(2, 5), true);
    test("A1A5B1B5C1C5D1D5E1", Point(3, 5), true);
    test("A1A5B1B5C1C5D1D5E1", Point(4, 5), true);
    test("A1A5B1B5C1C5D1D5E1", Point(5, 5), false);

    test("B1B5C1C5D1D5E1E5F1", Point(0, 5), false);
    test("B1B5C1C5D1D5E1E5F1", Point(1, 5), true);
    test("B1B5C1C5D1D5E1E5F1", Point(2, 5), true);
    test("B1B5C1C5D1D5E1E5F1", Point(3, 5), true);
    test("B1B5C1C5D1D5E1E5F1", Point(4, 5), true);
    test("B1B5C1C5D1D5E1E5F1", Point(5, 5), true);

    test("B1B5C1C5D1D5E1E5F1F5", Point(3, 1), true);

    // COLUMNS
    test("A1B1A2B2A3B3A4B4A5", Point(0, 0), false);
    test("A1B1A2B2A3B3A4B4A5", Point(0, 1), true);
    test("A1B1A2B2A3B3A4B4A5", Point(0, 2), true);
    test("A1B1A2B2A3B3A4B4A5", Point(0, 3), true);
    test("A1B1A2B2A3B3A4B4A5", Point(0, 4), true);
    test("A1B1A2B2A3B3A4B4A5", Point(0, 5), true);

    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 0), true);
    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 1), true);
    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 2), true);
    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 3), true);
    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 4), true);
    test("A6F6A2F2A3F3A4F4A5F5", Point(5, 5), false);

    // MAIN DIAGONAL
    test("A5A6B4B6C3C6D2D6E1", Point(0, 1), true);
    test("A5A6B4B6C3C6D2D6E1", Point(1, 2), true);
    test("A5A6B4B6C3C6D2D6E1", Point(2, 3), true);
    test("A5A6B4B6C3C6D2D6E1", Point(3, 4), true);
    test("A5A6B4B6C3C6D2D6E1", Point(4, 5), true);

    test("A6A1B5B1C4C1D3D1E2", Point(0, 0), true);
    test("A6A1B5B1C4C1D3D1E2", Point(1, 1), true);
    test("A6A1B5B1C4C1D3D1E2", Point(2, 2), true);
    test("A6A1B5B1C4C1D3D1E2", Point(3, 3), true);
    test("A6A1B5B1C4C1D3D1E2", Point(4, 4), true);
    test("A6A1B5B1C4C1D3D1E2", Point(5, 5), false);

    // OTHER DIAGONAL
    test("A1A6B2B6C3C6D4D6E5", Point(0, 5), true);
    test("A1A6B2B6C3C6D4D6E5", Point(1, 4), true);
    test("A1A6B2B6C3C6D4D6E5", Point(2, 3), true);
    test("A1A6B2B6C3C6D4D6E5", Point(3, 2), true);
    test("A1A6B2B6C3C6D4D6E5", Point(4, 1), true);
    test("A1A6B2B6C3C6D4D6E5", Point(5, 0), false);

    test("A2A6B3B6C4C6D5D6E6", Point(0, 4), true);
    test("A2A6B3B6C4C6D5D6E6", Point(1, 3), true);
    test("A2A6B3B6C4C6D5D6E6", Point(2, 2), true);
    test("A2A6B3B6C4C6D5D6E6", Point(3, 1), true);
    test("A2A6B3B6C4C6D5D6E6", Point(4, 0), true);

    std::cout << "Test finished!" << std::endl;
    return 0;
}