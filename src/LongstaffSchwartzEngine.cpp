#include "LongstaffSchwartzEngine.h"
#include <lapacke.h>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <fstream>

LongstaffSchwartzEngine::LongstaffSchwartzEngine(AssetSimulator&   s,
                                                 OptionPricer&     p,
                                                 int               n,
                                                 const std::string& csvFile)
    : sim_(s), pricer_(p), polyOrder_(n), csvFile_(csvFile) {}

LsmResult LongstaffSchwartzEngine::price() {
    util::Matrix paths = sim_.simulate();
    const auto& prm    = sim_.params();

    const int M = paths.rows;                 // steps + 1
    const int N = paths.cols;                 // paths
    const int k = polyOrder_ + 1;             // basis size 

    util::Matrix CF(M, N);
    std::vector<int> firstEx(N, M);           // first exercise step per path
    for (int j = 0; j < N; ++j)
        CF(M - 1, j) = pricer_.payoff(paths(M - 1, j));

    std::vector<double> X(N * k), Y(N);
    const double dt   = prm.maturity / prm.steps;
    const double disc = std::exp(-prm.rate * dt);

    /* backward induction */
    for (int i = M - 2; i >= 1; --i) {
        for (int j = 0; j < N; ++j) {
            double S    = paths(i, j);
            double sPow = 1.0;
            for (int d = 0; d < k; ++d) {          // build basis row
                X[j * k + d] = sPow;
                sPow *= S;
            }
            Y[j] = CF(i + 1, j) * disc;            // discounted continuation
        }
        std::vector<double> A = X;                 // DGELS overwrites A
        int info = LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N',
                                 N, k, 1,
                                 A.data(), k,
                                 Y.data(), 1);
        if (info) throw std::runtime_error("LAPACKE_dgels failed");

        /* Y[0..k‑1] are regression coefficients */
        for (int j = 0; j < N; ++j) {
            double S    = paths(i, j);
            double cont = 0.0, sPow = 1.0;
            for (int d = 0; d < k; ++d) {
                cont += Y[d] * sPow;
                sPow *= S;
            }
            double exer     = pricer_.payoff(S);
            bool   exercise = (exer > cont);
            CF(i, j)        = exercise ? exer : cont;
            if (exercise && firstEx[j] == M) firstEx[j] = i;
        }
    }

    /* option price */
    double priceSum = 0.0;
    for (int j = 0; j < N; ++j) priceSum += CF(1, j) * disc;
    double price = priceSum / N;

    /* expected exercise step & payoff */
    double stepSum = 0.0, paySum = 0.0;
    for (int j = 0; j < N; ++j) {
        int step = (firstEx[j] == M) ? M - 1 : firstEx[j];
        stepSum += step;
        paySum  += pricer_.payoff(paths(step, j)) * std::exp(-prm.rate * dt * step);
    }

    /* dump paths */
    std::ofstream csv(csvFile_);
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            csv << paths(i, j);
            if (j + 1 < N) csv << ',';
        }
        csv << '\n';
    }

    return { price,
             stepSum / N,
             paySum  / N };
}

