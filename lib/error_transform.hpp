#include <cmath>

double transform_value(double val) {
    const double most_important = asinh(10000);
    constexpr double width = 2; // most important are values around +/- 1k-100k
    val = asinh(val);
    val = tanh((val-most_important)/width) + tanh((val+most_important)/width);
    return val;
}