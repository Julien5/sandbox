#include "simulation.h"
#include "common/time.h"
#include "common/debug.h"
#include <cstdlib>
#include <cstring>
#include <vector>
#include "read_file.h"

namespace {
    std::string _getenv(const std::string &var) {
        const char *val = std::getenv(var.c_str());
        return val == nullptr ? std::string() : std::string(val);
    }
    struct Period {
        common::time::ms start_time;
        int period = 0;
    };
    std::vector<Period> read_periods() {
        static std::vector<Period> periods;
        if (periods.empty()) {
            auto c = read_file::content(_getenv("PERIODS_FILE"));
            auto L = read_file::split(c, "\n");
            for (auto line : L) {
                if (line.empty())
                    continue;
                auto E = read_file::split(line, ",");
                assert(E.size() == 2);
                Period p;
                auto time_strs = read_file::split(E[0], ":");
                int hours = 0, minutes = 0, seconds = 0;
                hours = std::stoi(time_strs[0]);
                if (time_strs.size() > 1)
                    minutes = std::stoi(time_strs[1]);
                if (time_strs.size() > 2)
                    seconds = std::stoi(time_strs[2]);
                p.start_time = common::time::ms((hours * 3600 + minutes * 60 + seconds) * 1000);
                p.period = std::stoi(E[1]);
                assert(p.period > 0);
                DBG("time:%zu period:%d\r\n", p.start_time.value(), p.period);
                periods.push_back(p);
            }
        }
        return periods;
    }
    int get_period(const common::time::ms &millis) {
        const auto P = read_periods();
        int period = P.front().period;
        for (auto p : P) {
            if (millis.value() < p.start_time.value())
                return period;
            period = p.period;
        }
        return period;
    }
}

int tick_period() {
    if (!_getenv("PERIODS_FILE").empty())
        return get_period(common::time::since_reset());
    auto env = _getenv("TICK_PERIOD");
    if (!env.empty())
        return std::stoi(env);
    return 480; // 100W @ 75U/kWh
}

common::time::ms simulation::duration() {
    if (!_getenv("SIMULATION_DURATION").empty())
        return common::time::ms(std::stoi(_getenv("SIMULATION_DURATION")));
    return common::time::ms(24 * 3600 * 1000);
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
