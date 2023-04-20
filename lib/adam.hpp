#pragma once

#include <stdint.h>

template <size_t N>
struct AdamOpt
{
    static constexpr double GRAD_MOMENTUM = 0.8;
    static constexpr double AVG_MOMENUTM = 0.8;
    static constexpr double AVGGRAD_MOMENTUM = 0.9;
    static constexpr double AVGGRAD_EPSILON = 1e-3;

    int64_t vals[N];
    double vals_d[N];
    double grad[N];
    double avg_vals[N];
    double avg_grad[N];

    AdamOpt(const int64_t init_vals[N]);
    void apply_gradient(double learning_rate);
};

template <size_t N>
AdamOpt<N>::AdamOpt(const int64_t init_vals[N])
{
    memcpy(&vals, init_vals, sizeof(int64_t)*N);

    for (int i = 0; i < N; i++)
    {
        vals_d[i] = (double)vals[i];
        avg_vals[i] = vals_d[i];
        grad[i] = 0;
        avg_grad[i] = 1e12;
    }
}

template <size_t N>
void AdamOpt<N>::apply_gradient(double learning_rate)
{
    for (int i = 0; i < N; i++)
    {
        // update values
        const double scale = avg_grad[i] + abs(grad[i]) * AVGGRAD_EPSILON + 1e-7;
        vals_d[i] += learning_rate * grad[i] / scale;

        // update average gradient
        avg_grad[i] = avg_grad[i] * AVGGRAD_MOMENTUM + abs(grad[i]) * (1 - AVGGRAD_MOMENTUM);

        // update average values
        avg_vals[i] = avg_vals[i] * AVG_MOMENUTM + abs(vals_d[i]) * (1 - AVG_MOMENUTM);
        
        // reset gradient
        grad[i] = 0;

        // update integer values
        vals[i] = (int64_t)vals_d[i];
    }
}