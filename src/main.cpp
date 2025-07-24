#include <cstdio>      
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AssetSimulator.h"
#include "ConfigLoader.h"
#include "LongstaffSchwartzEngine.h"
#include "OptionPricer.h"

static std::vector<double> run_price_fetcher(const std::string& symbol,
                                             int   lookback) {
    std::ostringstream cmd;
    cmd << "python3 scripts/price_fetcher.py "
        << symbol
        << " --lookback " << lookback;

    std::unique_ptr<FILE, decltype(&pclose)>
        pipe(popen(cmd.str().c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("Failed to run price_fetcher.py");

    std::vector<double> prices;
    char buf[128];
    while (fgets(buf, sizeof(buf), pipe.get())) {
        prices.push_back(std::stod(buf));
    }
    if (prices.size() < 2)
        throw std::runtime_error("price_fetcher.py returned <2 prices");
    return prices;
}

static double realised_vol(const std::vector<double>& p) {
    std::vector<double> logret;
    logret.reserve(p.size() - 1);
    for (size_t i = 1; i < p.size(); ++i)
        logret.push_back(std::log(p[i] / p[i - 1]));

    double mean = 0.0;
    for (double r : logret) mean += r;
    mean /= logret.size();

    double var = 0.0;
    for (double r : logret) var += (r - mean) * (r - mean);
    var /= (logret.size() - 1);

    return std::sqrt(var) * std::sqrt(252.0); 
}

int main(int argc, char** argv) {
    try {
        std::string jsonPath = (argc > 1) ? argv[1] : "../params.json";
        AppConfig cfg        = ConfigLoader::load(jsonPath);

        int lookback = (cfg.steps > 0) ? cfg.steps : 252;

        std::vector<double> closes =
            run_price_fetcher(cfg.symbol, lookback);

        double spot = closes.back();
        double vol  = realised_vol(closes);

        std::cout << "Fetched " << closes.size()
                  << " closes, spot=" << spot
                  << "  realised σ=" << vol << '\n';

        std::string csvName = cfg.symbol + "_" + cfg.startDate + "_"
                            + cfg.maturityDate + "_sim.csv";

        AssetSimulator sim({spot,            // override
                            vol,             // override
                            cfg.rate,
                            cfg.maturity,
                            cfg.steps,
                            cfg.paths});

        OptionPricer pricer((cfg.optionType == "PUT")
                                ? OptionPricer::PUT
                                : OptionPricer::CALL,
                            cfg.strike);

        LongstaffSchwartzEngine engine(sim, pricer,
                                       cfg.polyOrder,
                                       csvName);

        LsmResult out = engine.price();

        std::cout << "American option price : " << out.price              << '\n'
                  << "E[exercise step]      : " << out.expExerciseStep    << '\n'
                  << "E[exercise payoff]    : " << out.expExercisePayoff << '\n'
                  << "Simulated paths dumped to " << csvName << '\n';
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << '\n';
        return 1;
    }
}

