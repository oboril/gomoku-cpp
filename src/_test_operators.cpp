#include <iostream>
#include "board.hpp"
#include <unordered_map>

int main()
{
    constexpr int N = 6;

    Board boards[N] = {};
    boards[0].moves_from_string("c3d4d3b4c4c2e2");
    boards[1].moves_from_string("c3d4d3b4c4c2e2");
    boards[1].swap_player();
    boards[2].moves_from_string("c3d4d3b4c4c2e1");
    boards[3].moves_from_string("c3d4d3e4c4c2e2");
    boards[3].swap_player();
    boards[4].moves_from_string("c3d4d3b4c4c2");
    boards[5].moves_from_string("c3d4d3b4c4c2e2e3");

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if ((boards[i] == boards[j]) != (i == j))
            {
                std::cout << "Invalid comparison at (" << i << ", " << j << ")\n";
            }
        }
    }

    // Test hash collisions
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if ((boards[i].hash() == boards[j].hash()) != (i == j))
            {
                std::cout << "Hash collision at (" << i << ", " << j << ")\n";
            }
        }
    }

    // Test hashmap
    std::unordered_map<Board, char> hashtable;
    hashtable[boards[0]] = 'a';
    hashtable[boards[1]] = 'b';
    hashtable[boards[2]] = 'c';
    if (hashtable[boards[0]] != 'a' || hashtable[boards[1]] != 'b' || hashtable[boards[2]] != 'c') {
        std::cout << "Hash map does not work!!!\n";
    }

    std::cout << "Tests finished!" << std::endl;
}