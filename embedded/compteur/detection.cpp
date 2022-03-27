#include "detection.h"
#include "common/debug.h"
#include "common/time.h"
#include <math.h>
#include <string.h>
#include "simulation.h"

u16 bound(u16 T, u16 k) {
    return T + k;
}

Detection::Detection() {
}

bool new_value(const bool &last_value, const float &delta, const float &deltha_threshold) {
    if (last_value) {
        return delta > 0;
    }
    return delta > deltha_threshold;
}

bool Detection::tick() {
    return tick_worker();
}

bool Detection::adapt_threshold(const float &delta, float *threshold) {
    if (delta > 0)
        return false;
    auto seconds = float(common::time::since_reset().value()) / 1000;
    const float alpha = 0.999;
    if (m_delta_power == 0)
        m_delta_power = -delta;
    m_delta_power = alpha * m_delta_power + (1 - alpha) * (-delta);
    if (seconds < 15)
        return false;
    *threshold = 4 * m_delta_power;
    PLOT("threshold:%f:%f:%f\r\n", seconds, m_threshold, -m_delta_power);
    return true;
}

bool Detection::tick_worker() {
#ifdef SIMULATION
    return simulation::tick();
#else
    u16 value = 0;
    if (!m_reader.tick(&value))
        return false;

    float delta = 0;
    const float alpha = 0.9875;
    if (xalpha < 0)
        xalpha = value;
    else {
        xalpha = alpha * xalpha + (1 - alpha) * value;
        delta = xalpha - value;
    }
    auto seconds = float(common::time::since_reset().value()) / 1000;
    PLOT("values:%f:%d:%f\r\n", seconds, int(value), xalpha);
    PLOT("delta:%f:%f\r\n", seconds, delta);
    PLOT("variance:%f:%f\r\n", seconds, variance_delta);

    LOG("[%lu]-> value:%d xalpha:%d delta:%03d threshold:%03d\r\n", u32(common::time::since_reset().value()), int(value), int(xalpha), int(delta), int(m_threshold));
    //DBG("[%d]->%d \r\n", int(common::time::since_reset().value()), int(value));
    adapt_threshold(delta, &m_threshold);
    const auto v2 = new_value(m_last_value, delta, m_threshold);
    if (v2 == m_last_value)
        return false;
    m_last_value = v2;
    if (!v2) {
        return false;
    }
    PLOT("ticks:%f:%d\r\n", seconds, value);
    LOG("[%lu]-> ticked:%d\r\n", common::time::since_reset().value(), int(value));
#endif
    return true;
}

#ifdef PC
int Detection::test() {
    return 0;
}
#endif
