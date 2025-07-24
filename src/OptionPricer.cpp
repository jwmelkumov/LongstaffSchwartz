#include "OptionPricer.h"
#include <algorithm>
#include <stdexcept>

OptionPricer::OptionPricer(Type t, double K) : type_(t), strike_(K) {
    if (K < 0) throw std::invalid_argument("Strike must be non‑negative");
}

double OptionPricer::payoff(double S) const noexcept {
    return (type_ == CALL) ? std::max(S - strike_, 0.0) : std::max(strike_ - S, 0.0);
}

