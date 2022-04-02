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
        return delta < 0;
    }
    return delta < -deltha_threshold;
}

bool Detection::tick() {
    return tick_worker();
}

bool Detection::adapt_threshold(const float &delta, float *threshold) {
    if (delta < 0) {
        m_delta_max = xMax(m_delta_max, -delta);
        return false;
    }
    auto seconds = float(common::time::since_reset().value()) / 1000;
    float alpha = 0.999;
    m_delta_mean = alpha * m_delta_mean + (1 - alpha) * delta;
    alpha = 0.9999;
    m_delta_max = alpha * m_delta_max + (1 - alpha) * m_delta_mean;
    if (seconds < 15)
        return false;
    *threshold = (m_delta_mean + m_delta_max) / 2;
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
        delta = value - xalpha;
    }
    auto seconds = float(common::time::since_reset().value()) / 1000;

    LOG("[%07lu] value:%03d xalpha:%03d delta:%03d deltamax:%03d threshold:%03d\r\n", u32(common::time::since_reset().value()), int(value), int(xalpha), int(delta), int(-m_delta_max), int(-m_threshold));
    //DBG("[%d]->%d \r\n", int(common::time::since_reset().value()), int(value));
    adapt_threshold(delta, &m_threshold);
    const auto v2 = new_value(m_last_value, delta, m_threshold);
    if (v2 == m_last_value)
        return false;
    m_last_value = v2;
    if (!v2) {
        return false;
    }
    LOG("[%07lu] ticked:%03d\r\n", u32(common::time::since_reset().value()), int(value));
    return true;
#endif
}

#ifdef PC
int Detection::test() {
    return 0;
}
#endif
