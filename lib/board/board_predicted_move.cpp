#include "board.hpp"

bool Board::play_predicted_move(const EvaluationTable *predict_table)
{
    auto moves = get_moves(predict_table);
    Move best_move(Point(0, 0), LOSS);
    for (Move m : moves)
    {
        if (m.score > best_move.score)
        {
            best_move = m;
        }
    }
    play(best_move.point);
    return check_win(best_move.point);
}