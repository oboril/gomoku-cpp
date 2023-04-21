#include <stdint.h>

struct Point
{
    int8_t x;
    int8_t y;
    Point(int8_t x, int8_t y) : x(x), y(y) {}
    Point() : x(0), y(0) {}
    bool operator==(const Point &other) const { return (x == other.x) && (y == other.y); }
};

struct Move
{
    Point point;
    int64_t score;
    Move(Point p, int64_t s) : score(s) { point = p; }
    Move() : score(0) { point = Point(-1, -1); }
};