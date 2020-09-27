#include "compteur.h"
#include "common/debug.h"
#include "common/time.h"
#include "analog.h"
#include "status.h"
#include <math.h>
#include <string.h>

bool TicksReader::calibrated(u16 *_TL, u16 *_TH) const {
    const u8 minWidth = 4;
    const auto M = H.maximum();
    const auto m = H.minimum();
    status::instance.set(status::index::M, M);
    status::instance.set(status::index::m, m);
    status::instance.set(status::index::line, __LINE__);
    if (M - m < minWidth)
        return false;
    status::instance.set(status::index::line, __LINE__);
    const auto T0 = H.threshold(5);
    status::instance.set(status::index::T0, T0);
    if (T0 == m || T0 == M)
        return false;
    status::instance.set(status::index::line, __LINE__);
    const auto v1 = H.argmax(m, T0 - 1);
    const auto v2 = H.argmax(T0, M);
    status::instance.set(status::index::v1, v1);
    status::instance.set(status::index::v2, v2);
    if (v1 == v2)
        return false;
    status::instance.set(status::index::line, __LINE__);
    const auto v = H.argmin(v1, v2);
    status::instance.set(status::index::v, v);
    if (v == v1 || v == v2)
        return false;
    status::instance.set(status::index::line, __LINE__);
    const auto TH = v + 1;
    const auto TL = v - 1;
    status::instance.set(status::index::TH, TH);
    status::instance.set(status::index::TL, TL);
    *_TL = TL;
    *_TH = TH;
    return true;
}

u16 analog_read() {
#ifdef PC
    return analog().read();
#else
    return common::analog().read();
#endif
}

bool TicksReader::take() {
    const auto a = analog_read();
    DBG("time:%d s analog value:%d\r\n", int(common::time::since_reset()), int(a));
    H.update(a);
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index >= size_adc) {
        memset((u8 *)m_last_adc_value, 0, sizeof(m_last_adc_value));
        m_adc_index = 0;
    }
    m_last_adc_value[m_adc_index] = a;
    m_adc_index++;
    // H.print();
    u16 TH = 0;
    u16 TL = 0;
    assert(TL <= TH);
    auto c = calibrated(&TL, &TH);
    status::instance.set(status::index::calibrated, u8(c));
    if (!c) {
        return false;
    }
    //DBG("TL=[%3d] TH=[%3d]\r\n", TL, TH);
    // is the value classificable ?
    if (TL < a && a < TH) {
        return false;
    }
    const auto new_value = a >= TH;
    if (new_value == m_last_value)
        return false;
    m_last_value = new_value;
    if (new_value == 0) {
        return false;
    }
    return true;
}

const u8 *TicksReader::histogram_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    DBG("m_adc_index:%d size:%d\r\n", m_adc_index, size_adc);
    if (m_adc_index < size_adc)
        return 0;
    return (u8 *)H.get_packed(L);
}

const u8 *TicksReader::adc_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index < size_adc)
        return 0;
    *L = sizeof(m_last_adc_value);
    return (u8 *)(&m_last_adc_value);
}
