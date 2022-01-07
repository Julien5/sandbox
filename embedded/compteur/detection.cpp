#include "detection.h"
#include "common/debug.h"
#include "common/time.h"
#include "status.h"
#include <math.h>
#include <string.h>

u16 bound(u16 T, u16 k) {
    return T + k;
}

Detection::Detection() {
}

bool Detection::tick() {
    u16 value = 0;
    if (!m_reader.tick(&value))
        return false;
    float delta = 0;
    const float alpha = 0.9875;
    if (xalpha < 0)
        xalpha = value;
    else {
        if (delta > 3) {
            const float a = 0.96;
            variance_delta = a * variance_delta + (1 - a) * delta * delta;
        }
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

    const auto new_value = value < xalpha && delta > 85;
    if (new_value == m_last_value)
        return false;
    m_last_value = new_value;
    if (new_value == 0) {
        return false;
    }
    PLOT("ticks:%f:%d\r\n", seconds, value);
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
