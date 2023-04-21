#include <cmath>

double transform_value(double val) {
    //const double most_important = std::asinh(10000.);
    //constexpr double width = 2.; // most important are values around +/- 1k-100k
    //val = std::asinh(val);
    //val = std::tanh((val-most_important)/width) + std::tanh((val+most_important)/width);
    //return val;

    //return std::tanh(std::asinh(val/100.)/8.);
    return std::tanh(std::asinh(val)/9.);

    //return std::asinh(val/100.);
}