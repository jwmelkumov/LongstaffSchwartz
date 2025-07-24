#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct AppConfig {
    double spot{100}, vol{0.2}, rate{0.05};
    double maturity{1.0}; 
    int steps{50}, paths{10000};
    double strike{100};
    std::string optionType{"CALL"};
    int polyOrder{2};

    std::string symbol{"AAPL"};
    std::string startDate{"2025-07-21"};
    std::string maturityDate{"2026-07-21"};
};

class ConfigLoader {
public:
    static AppConfig load(const std::string& path);
};

