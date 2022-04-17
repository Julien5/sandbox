#include "detection.h"
#include "common/debug.h"
#include "common/time.h"
#include <math.h>
#include <string.h>
#include "simulation.h"
#include "softdebug.h"

u16 bound(u16 T, u16 k) {
    return T + k;
}

Detection::Detection() {
}

bool Detection::trigger(const float &delta) {
    if (m_delta_old == 0)
        return false;
    const auto T = -m_threshold;
    const auto &d1 = m_delta_old - T;
    const auto &d2 = delta - T;
    if (d1 * d2 < 0) { // cross T
        m_trigger_conditions[0] = d1 < 0 && d2 > 0;
    }
    if (m_delta_old * delta < 0) { // cross 0
        m_trigger_conditions[1] = m_delta_old < 0 && delta > 0;
    }
    bool ret = m_trigger_conditions[0] && m_trigger_conditions[1];
    if (ret) {
        m_trigger_conditions[0] = false;
        m_trigger_conditions[1] = false;
    }
    return ret;
}

bool Detection::tick() {
    return tick_worker();
}

bool Detection::adapt_threshold(const float &delta, float *threshold) {
    if (delta < 0) {
        // the red mark may be here
        m_delta_max = xMax(m_delta_max, -delta);
        return false;
    }
    // the red mark is probably not here
    auto seconds = float(common::time::since_reset().value()) / 1000;
    float alpha = 0.999;
    m_delta_mean = alpha * m_delta_mean + (1 - alpha) * delta;
    alpha = 0.99999;
    m_delta_max = alpha * m_delta_max + (1 - alpha) * m_delta_mean;
    if (seconds < 15)
        return false;
    *threshold = (2 * m_delta_mean + 2 * m_delta_max) / 4;
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
    const float alpha = 0.999;
    if (xalpha < 0)
        xalpha = value;
    else {
        xalpha = alpha * xalpha + (1 - alpha) * value;
        delta = value - xalpha;
    }
    auto seconds = float(common::time::since_reset().value()) / 1000;

    if (softdebug::log_enabled()) {
        LOG("[%07lu] x:%03d xa:%03d d:%03d dmax:%03d T:%03d\r\n", u32(common::time::since_reset().value()), int(value), int(xalpha), int(delta), int(-m_delta_max), int(-m_threshold));
    }
    //DBG("[%d]->%d \r\n", int(common::time::since_reset().value()), int(value));
    adapt_threshold(delta, &m_threshold);
    auto ticked = trigger(delta);
    m_delta_old = delta;
    if (ticked && softdebug::log_enabled())
        LOG("[%07lu] ticked:%03d\r\n", u32(common::time::since_reset().value()), int(value));
    return ticked;
#endif
}

#ifdef PC
int Detection::test() {
    return 0;
}
#endif
