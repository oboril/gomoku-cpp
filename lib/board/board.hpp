#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#define SIGN(a) (((a) > 0) - ((a) < 0))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define BOARD_SIZE 8
constexpr size_t BOARD_SIZE_ALLOC = ((BOARD_SIZE + 1) / 2) * 2;
using BoardRow = uint16_t;
#define EVAL_TABLE_SIZE 12
using EvaluationTable = int64_t[EVAL_TABLE_SIZE];

// WIN = 10^12
#define WIN 1000000000000
#define LOSS -WIN

// optimized on board size 10
static constexpr EvaluationTable DEFAULT_EVAL_TABLE = {30, 400, 1000, 3800, 43000, WIN, 500, -300, -800, -1900, -5500, LOSS};
static constexpr EvaluationTable DEFAULT_PREDICT_TABLE = {400, 400, 3000, 40000, WIN/10, WIN, -500, 300, 700, 3000, 37000, LOSS};
 
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

    static Board random(int iters);

    bool play_predicted_move(const EvaluationTable * predict_table);
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

    inline bool get_player() const {return m_player; }

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