#pragma once
#include <vector>

namespace util {
    struct Matrix {
        int rows, cols;
        std::vector<double> data; 
        Matrix(int r = 0, int c = 0) : rows(r), cols(c), data(r * c, 0.0) {}
        double& operator()(int i, int j)       noexcept { return data[i * cols + j]; }
        double  operator()(int i, int j) const noexcept { return data[i * cols + j]; }
    };
}

class AssetSimulator {
public:
    struct Params {
        double spot, vol, rate, maturity; 
        int steps, paths;
    };

    explicit AssetSimulator(const Params& p);
    util::Matrix simulate();
    [[nodiscard]] const Params& params() const noexcept { return params_; }
private:
    Params params_;
};

