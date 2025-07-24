#include <cassert>
#include <iostream>
#include "AssetSimulator.h"
#include "OptionPricer.h"
#include "LongstaffSchwartzEngine.h"

int main() {
    /* --- minimal simulation parameters --- */
    AssetSimulator sim({100.0,   // spot
                        0.2,     // vol
                        0.05,    // rate
                        1.0,     // maturity
                        3,       // steps
                        50});    // paths

    OptionPricer pricer(OptionPricer::CALL, 100.0);

    /* polyOrder = 2, CSV written to dummy.csv (ignored in test) */
    LongstaffSchwartzEngine eng(sim, pricer, 2, "dummy.csv");

    LsmResult res = eng.price();
    assert(res.price >= 0.0 && res.price <= 100.0);

    std::cout << "Engine Test Passed — price=" << res.price << '\n';
    return 0;
}

