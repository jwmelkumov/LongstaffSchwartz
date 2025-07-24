#include "ConfigLoader.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using nlohmann::json;

/* convert YYYY‑MM‑DD → time_t (midnight, UTC) -------------------------- */
static std::time_t iso_to_time_t(const std::string& iso) {
    std::tm tm{};
    std::istringstream ss(iso);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) throw std::runtime_error("Bad date: " + iso);
#if defined(_WIN32)
    return _mkgmtime(&tm);          
#else
    return timegm(&tm);            
#endif
}

AppConfig ConfigLoader::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open config file " + path);

    json j; f >> j;
    AppConfig c;

#define MAP(KEY, FIELD) if (j.contains(KEY)) c.FIELD = j[KEY].get<decltype(c.FIELD)>()
    MAP("spot", spot);
    MAP("vol",  vol);
    MAP("rate", rate);

    MAP("steps", steps);
    MAP("paths", paths);
    MAP("strike", strike);
    MAP("optionType", optionType);
    MAP("polyOrder", polyOrder);

    MAP("symbol",       symbol);
    MAP("startDate",    startDate);
    MAP("maturityDate", maturityDate);
#undef MAP

    std::time_t t0 = iso_to_time_t(c.startDate);
    std::time_t t1 = iso_to_time_t(c.maturityDate);
    if (t1 <= t0) throw std::runtime_error("maturityDate must be after startDate");

    double days = std::difftime(t1, t0) / 86400.0;      // seconds → days
    c.maturity  = days / 365.0;

    /* default = 252 */
    if (c.steps == 0) c.steps = 252;

    return c;
}

