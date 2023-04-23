#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "move.hpp"

#define SIGN(a) (((a) > 0) - ((a) < 0))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define BOARD_SIZE 15
constexpr size_t BOARD_SIZE_ALLOC = ((BOARD_SIZE + 1) / 2) * 2;
using BoardRow = uint16_t;
#define EVAL_TABLE_SIZE 32
#define PRED_TABLE_SIZE 12
using EvaluationTable = int64_t[EVAL_TABLE_SIZE];
using PredictionTable = int64_t[PRED_TABLE_SIZE];

// WIN = 10^12
#define WIN ((int64_t)1000000000000)
#define LOSS (-WIN)
// any move with move score > 10^9 is forced, other moves need not be tried
#define FORCING ((int64_t)1000000000)

// optimized on board size 10
// static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {10, 240, 700, 1600, 300000, WIN, 500, -210, -540, -1200, -3300, LOSS};
// static constexpr EvaluationTable DEFAULT_PREDICT_TABLE = {220, 300, 700, 1800,FORCING*100, WIN, 0, 250, 600, 1500, FORCING*10, LOSS};

// optimized on board size 15
// static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {77, 3691, 16233, 42646, 10000000, WIN, 47139, -4269, -13974, -36452, -109778, LOSS};
static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {17, 3348, 16050, 65109, WIN/10, WIN, 56966, -2333, -13498, -50243, -299474, LOSS, 68, 57, -12, 6215, -4144, -8966, -47748, -1396, 532, 49, 75, -2314, -33, 934, -5767, -9688, -9514, -17152, -16229, -317205};
static constexpr PredictionTable DEFAULT_PREDICT_TABLE = {3446, 10386, 18377, 67042, FORCING * 100, WIN, -215, 4244, 16313, 40680, FORCING * 10, LOSS};


class Board
{
public:
    Board();
    Board(const Board &rhs);

    static Board random(int iters);

    bool play_predicted_move(const PredictionTable *predict_table);
    bool moves_from_string(const std::string &moves);
    void set(Point p, bool player);
    void reset_move(Point p);
    void swap_player();
    void play(Point p);
    bool play_checked(Point p);
    bool is_empty(Point p) const;
    void print() const;
    void print_highlight(Point p) const;
    void debug() const;
    bool check_win(Point p) const;

    int64_t evaluate(const EvaluationTable *eval_table) const;
    std::vector<Move> get_moves(const PredictionTable *eval_table) const;

    inline bool get_player() const { return m_player; }

    bool operator==(const Board &rhs) const;
    bool operator!=(const Board &rhs) const { return !operator==(rhs); };

    size_t hash() const;

private:
    BoardRow m_board[2][BOARD_SIZE_ALLOC];
    bool m_player; // true = crosses to play
};

template <>
struct std::hash<Board>
{
    size_t operator()(const Board &b) const { return b.hash(); }
};