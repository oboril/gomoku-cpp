#include "board.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    { // TESTCASE
        EvaluationTable eval_table = {
            0, 0, 0, 0, 0, 0, // P1 counts
            0, 0, 0, 0, 0, 0, // bias + P2 counts
            0, 0, 0, 0,       // P1 11 22 33 44
            0, 0, 0, 0,       // P2 11 22 33 44
            0, 0, 0, 0, 0, 0, // P1 21 31 32 41 42 43
            0, 0, 0, 0, 0, 0  // P2 21 31 32 41 42 43
        };
        Board b;
        b.moves_from_string("h9d12h8e12h7c12g6f12f6k12e6");
        cout << b.evaluate(&eval_table) << endl;
    }

    { // TESTCASE
        EvaluationTable eval_table = {
            0, 0, 1, 1, 1, 1, // P1 counts
            0, 0, 1, 1, 1, 1, // bias + P2 counts
            0, 0, 1, 1,       // P1 11 22 33 44
            0, 0, 1, 1,       // P2 11 22 33 44
            1, 1, 1, 1, 1, 1, // P1 21 31 32 41 42 43
            1, 1, 1, 1, 1, 1  // P2 21 31 32 41 42 43
        };
        Board b;
        b.moves_from_string("h8a1");
        cout << b.evaluate(&eval_table) << endl;
    }

    { // TESTCASE
        EvaluationTable eval_table = {
            0, 0, 0, 0, 100, 0, // P1 counts
            0, 0, 0, 0, 0, 0, // bias + P2 counts
            0, 0, 0, 0,       // P1 11 22 33 44
            0, 0, 1, 0,       // P2 11 22 33 44
            0, 0, 0, 0, 0, 0, // P1 21 31 32 41 42 43
            0, 0, 0, 0, 0, 0  // P2 21 31 32 41 42 43
        };
        Board b;
        b.moves_from_string("h9d12h8e12h7c12g6f12f6k12e6"); // x=8, y=10
        cout << b.evaluate(&eval_table) << endl;
    }

    { // TESTCASE
        EvaluationTable eval_table = {
            0, 0, 0, 0, 0, 0, // P1 counts
            0, 0, 0, 0, 0, 0, // bias + P2 counts
            0, 0, 0, 0,       // P1 11 22 33 44
            0, 0, 0, 0,       // P2 11 22 33 44
            0, 0, 0, 0, 0, 0, // P1 21 31 32 41 42 43
            0, 0, 0, 0, 0, 1  // P2 21 31 32 41 42 43
        };
        Board b;
        b.moves_from_string("e9l13f8m11g7b3i7f1g5m8j8d14f4"); // x=8, y=10
        cout << b.evaluate(&eval_table) << endl;
    }

    cout << "Finished!" << endl;
    return 0;
}