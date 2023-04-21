#include "board.hpp"
#include "minimax.hpp"
#include "colors.hpp"
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

Point get_move(Board & b)
{
    string input;
    while (true)
    {
        cout << BOLDWHITE << "Write your move (e.g. f8): " << RESET;
        cin >> input;

        if (input.length() == 2) {
            const char letter = input[0];
            const char number = input[1];
            if (letter <= 'o' && letter >= 'a' && number > '0' && number <= '9')
            {
                Point p(letter-'a', BOARD_SIZE - (number-'0'));
                if (b.is_empty(p))
                {
                    return p;
                }
                else 
                {
                    cout << RED << "Cannot play here\n" << RESET;
                    continue;
                }
            }
        }
        else if (input.length() == 3) {
            const char letter = input[0];
            const char number = input[2];
            if (letter <= 'o' && letter >= 'a' && number >= '0' && number <= '9' && input[1] == '1')
            {
                Point p(letter-'a', BOARD_SIZE - (number-'0'+10));
                if (b.is_empty(p))
                {
                    return p;
                }
                else 
                {
                    cout << RED << "Cannot play here\n" << RESET;
                    continue;
                }
            }
        }
        cout << RED << "Invalid format!\n" << RESET;
    }
}

void print_eval(int64_t eval)
{
    cout << "Evaluation: ";
    if (eval < LOSS+100)
    {
        cout << "M" << (eval-LOSS);
    }
    else if (eval > WIN-100)
    {
        cout << "-M" << (WIN-eval);
    }
    else {
        cout << round(std::asinh(-(double)eval/100.)*10.)/10.;
    }

    cout << endl;
}

int main()
{
    Board b;
    negamax::TranspTable transp_table;
    b.print();
    while(true)
    {
        // let player make a move
        Point p = get_move(b);
        b.play(p);

        if (b.check_win(p))
        {
            cout << BOLDGREEN<<"YOU WON !!!"<<RESET<<endl;
            break;
        }
        if (b.get_moves(&DEFAULT_PREDICT_TABLE).size() == 0)
        {
            cout << BOLDYELLOW<<"IT'S A DRAW !!!"<<RESET<<endl;
            break;
        }

        b.print_highlight(p);

        // calculate best move for computer
        int64_t iters = 0;
        int64_t eval = negamax::play_computer(b, 8, &DEFAULT_EVAL_TABLE, &DEFAULT_PREDICT_TABLE, transp_table, &iters, &p);
        print_eval(eval);
        b.play(p);

        if (b.check_win(p))
        {
            cout << BOLDRED<<"COMPUTER WON !!!"<<RESET<<endl;
            break;
        }
        if (b.get_moves(&DEFAULT_PREDICT_TABLE).size() == 0)
        {
            cout << BOLDYELLOW<<"IT'S A DRAW !!!"<<RESET<<endl;
            break;
        }

        b.print_highlight(p);
    }
    return 0;
}