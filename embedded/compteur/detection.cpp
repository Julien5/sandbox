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

bool new_value(const bool &last_value, const u16 &x, const u16 &x_alpha, const u16 &deltha_threshold) {
    if (last_value) {
        return x < x_alpha;
    }
    const auto delta = x_alpha - x;
    return x < x_alpha && delta > deltha_threshold;
}

bool Detection::tick() {
    auto ret = tick_worker();
    if (ret)
        m_last_tick_time = common::time::since_reset();
    return ret;
}

bool Detection::just_ticked() {
    if (m_last_tick_time.value() == 0) {
        //  m_last_tick_time = common::time::since_reset();
    }
    auto since = common::time::since_reset().since(m_last_tick_time);
    const u32 s15 = 1000 * 15;
    if (since.value() < s15)
        return true;
    const u32 s3600 = 1000 * 3600;
    if (since.value() > s3600)
        return true;
    return false;
}

bool Detection::adapt_threshold(const u16 &x, const float &x_alpha, u16 *threshold) {
    if (!just_ticked()) {
        m_threshold_max = 0;
        m_threshold_min = 0xffff;
        return false;
    }
    m_threshold_max = xMax(m_threshold_max, x);
    m_threshold_min = xMin(m_threshold_min, x);
    const u16 K = 3;
    auto seconds = float(common::time::since_reset().value()) / 1000;
    *threshold = xMin((m_threshold_max - m_threshold_min), 200);
    PLOT("threshold:%f:%d:%d:%d\r\n", seconds, m_threshold, m_threshold_min, m_threshold_max);
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

    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index >= size_adc) {
        memset((u8 *)m_last_adc_value, 0, sizeof(m_last_adc_value));
        m_adc_index = 0;
    }
    m_last_adc_value[m_adc_index] = value;
    m_adc_index++;

    DBG("[%d]-> value:%d xalpha:%d delta:%03d\r\n", int(common::time::since_reset().value() / 1000), int(value), int(xalpha), int(delta));
    //DBG("[%d]->%d \r\n", int(common::time::since_reset().value()), int(value));
    adapt_threshold(value, xalpha, &m_threshold);
    const auto v2 = new_value(m_last_value, value, xalpha, m_threshold);
    if (v2 == m_last_value)
        return false;
    m_last_value = v2;
    if (!v2) {
        return false;
    }
    PLOT("ticks:%f:%d\r\n", seconds, value);
#endif
    return true;
}

const u8 *Detection::histogram_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    DBG("m_adc_index:%d size:%d\r\n", m_adc_index, int(size_adc));
    if (m_adc_index < size_adc)
        return 0;
    return 0;
}

const u8 *Detection::adc_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index < size_adc)
        return 0;
    if (L)
        *L = sizeof(m_last_adc_value);
    return (u8 *)(&m_last_adc_value);
}

#ifdef PC
int Detection::test() {
    return 0;
}
#endif
