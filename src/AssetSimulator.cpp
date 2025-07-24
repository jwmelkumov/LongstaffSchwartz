#include "AssetSimulator.h"
#include <random>
#include <stdexcept>
#include <cmath>

AssetSimulator::AssetSimulator(const Params& p) : params_(p) {
    if (p.spot <= 0 || p.vol < 0 || p.maturity <= 0 || p.steps <= 0 || p.paths <= 0)
        throw std::invalid_argument("Invalid AssetSimulator parameters");
}

util::Matrix AssetSimulator::simulate() {
    int M = params_.steps + 1;
    int N = params_.paths;
    util::Matrix paths(M, N);

    std::mt19937_64 rng(std::random_device{}());
    std::normal_distribution<double> Z(0.0, 1.0);

    for (int j = 0; j < N; ++j) paths(0, j) = params_.spot;

    double dt = params_.maturity / params_.steps;
    double mu = (params_.rate - 0.5 * params_.vol * params_.vol) * dt;
    double sigma = params_.vol * std::sqrt(dt);

    for (int i = 1; i < M; ++i)
        for (int j = 0; j < N; ++j)
            paths(i, j) = paths(i - 1, j) * std::exp(mu + sigma * Z(rng));

    return paths;
}

