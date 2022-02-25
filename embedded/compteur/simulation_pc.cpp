#include "simulation.h"
#include "common/time.h"
#include "common/debug.h"
#include <cstdlib>
#include <cstring>

namespace {
    std::string _getenv(const std::string &var) {
        const char *val = std::getenv(var.c_str());
        return val == nullptr ? std::string() : std::string(val);
    }
}

int tick_period() {
    auto env = _getenv("TICK_PERIOD");
    if (env.empty())
        return 4;
    return std::stoi(env);
}

bool simulation::tick() {
    static bool last_value = false;
    auto t = common::time::since_reset();
    auto seconds = t.value() / 1000;
    if (seconds == 0)
        return false;
    auto new_value = seconds % tick_period() == 0;
    auto ret = last_value != new_value && new_value == true;
    last_value = new_value;
    return ret;
}
