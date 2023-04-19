#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#define SIGN(a) (((a) > 0) - ((a) < 0))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define BOARD_SIZE 10
constexpr size_t BOARD_SIZE_ALLOC = ((BOARD_SIZE + 1) / 2) * 2;
using BoardRow = uint16_t;
using EvaluationTable = int64_t[12];

// WIN = 10^12
#define WIN 1000000000000
#define LOSS -WIN

// OPTIMIZED FOR BOARD OF SIZE 6 !!!
//static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {1, 800, 2590, 10000, 10000000, WIN, 0, -285, -1380, -2660, -9000, LOSS};
//static constexpr EvaluationTable DEFAULT_PREDICT_TABLE = {1, 600, 2300, 10000, 10000000, WIN, 0, -800, -1000, -2500, -9000, LOSS};
//static constexpr EvaluationTable DEFAULT_PREDICT_TABLE = {1, 370, 1000, 8000, 10000000, WIN, 0, -300, -1000, -2000, -9000, LOSS};

static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {1, -100, 1000, 2800, 10000000, WIN, 0, 150, -650, -2400, -5000, LOSS};
static constexpr EvaluationTable DEFAULT_PREDICT_TABLE = {1, 0, 0, 0, 10000000, WIN, 0, 0, 0, 0, 0, LOSS};

struct Point
{
    int8_t x;
    int8_t y;
    Point(int8_t x, int8_t y) : x(x), y(y) {}
    Point() : x(0), y(0) {}
};

struct Move
{
    Point point;
    int64_t score;
    Move(Point p, int64_t s) : score(s) { point = p; }
};

class Board
{
public:
    Board();
    Board(const Board &rhs);

    bool moves_from_string(const std::string &moves);
    void set(Point p, bool player);
    void reset_move(Point p);
    void swap_player();
    void play(Point p);
    bool play_checked(Point p);
    bool is_empty(Point p) const;
    void print() const;
    void debug() const;
    bool check_win(Point p) const;
    int64_t evaluate(const EvaluationTable *eval_table) const;
    std::vector<Move> get_moves(const EvaluationTable *eval_table) const;

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