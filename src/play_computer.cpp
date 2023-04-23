#include "board.hpp"
#include "minimax.hpp"
#include "colors.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <limits>
using namespace std;

Point get_move(Board &b)
{
    string input;
    while (true)
    {
        cout << BOLDWHITE << "Write your move (e.g. f8): " << RESET;
        cin >> input;

        if (input.length() == 2)
        {
            const char letter = input[0];
            const char number = input[1];
            if (letter <= 'o' && letter >= 'a' && number > '0' && number <= '9')
            {
                Point p(letter - 'a', BOARD_SIZE - (number - '0'));
                if (b.is_empty(p))
                {
                    return p;
                }
                else
                {
                    cout << RED << "Cannot play here\n"
                         << RESET;
                    continue;
                }
            }
        }
        else if (input.length() == 3)
        {
            const char letter = input[0];
            const char number = input[2];
            if (letter <= 'o' && letter >= 'a' && number >= '0' && number <= '9' && input[1] == '1')
            {
                Point p(letter - 'a', BOARD_SIZE - (number - '0' + 10));
                if (b.is_empty(p))
                {
                    return p;
                }
                else
                {
                    cout << RED << "Cannot play here\n"
                         << RESET;
                    continue;
                }
            }
        }
        cout << RED << "Invalid format!\n"
             << RESET;
    }
}

int get_depth()
{
    int depth;
    while (true)
    {
        cout << BOLDWHITE << "Choose difficulty (recommended 5-8): " << RESET;
        cin >> depth;

        if (cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << RED << "Incorrect format!" << RESET << "\n";
            continue;
        }
        if (depth < 1 || depth > 12)
        {
            std::cout << RED << "The difficulty is out of allowed range!" << RESET << "\n";
            continue;
        }
        return depth;
    }
}

void print_eval(int64_t eval)
{
    cout << "Evaluation: ";
    if (eval < LOSS + 100)
    {
        cout << "M" << (eval - LOSS);
    }
    else if (eval > WIN - 100)
    {
        cout << "-M" << (WIN - eval);
    }
    else
    {
        cout << round(std::asinh(-(double)eval / 100000.) * 10.) / 10.;
    }

    cout << endl;
}

int main()
{
    const int depth = get_depth();

    Board b;
    negamax::TranspTable transp_table;
    b.print();
    while (true)
    {
        // let player make a move
        Point p = get_move(b);
        b.play(p);
        b.print_highlight(p);

        if (b.check_win(p))
        {
            cout << BOLDGREEN << "YOU WON !!!" << RESET << endl;
            break;
        }
        if (b.get_moves(&DEFAULT_PREDICT_TABLE).size() == 0)
        {
            cout << BOLDYELLOW << "IT'S A DRAW !!!" << RESET << endl;
            break;
        }

        // calculate best move for computer
        int64_t iters = 0;
        int64_t eval = negamax::play_computer(b, depth, &DEFAULT_EVAL_TABLE, &DEFAULT_PREDICT_TABLE, transp_table, &iters, &p);
        print_eval(eval);
        b.play(p);
        b.print_highlight(p);

        if (b.check_win(p))
        {
            cout << BOLDRED << "COMPUTER WON !!!" << RESET << endl;
            break;
        }
        if (b.get_moves(&DEFAULT_PREDICT_TABLE).size() == 0)
        {
            cout << BOLDYELLOW << "IT'S A DRAW !!!" << RESET << endl;
            break;
        }
    }
    cout << "Press enter to exit..." << endl;
    cin.ignore();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
}