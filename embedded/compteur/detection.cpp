#include "detection.h"
#include "common/debug.h"
#include "common/time.h"
#include "histogram.h"
#include "status.h"
#include <math.h>
#include <string.h>

u16 bound(u16 T, u16 k) {
    return T + k;
}

bool calibrated(histogram::Histogram H, u16 *_TL, u16 *_TH) {
    //H.print();
    const u8 minWidth = 10;
    const auto M = H.maximum();
    const auto m = H.minimum();
    //assert(M != 0);
    status::instance.set(status::index::M, M);
    status::instance.set(status::index::m, m);
    status::instance.set(status::index::line, __LINE__);
    DBG("m:%d M:%d\n", int(m), int(M));
    const auto d = M - m;
    if (d < float(m) * (5.0 / 100)) {
        DBG("ERR:not calibrated (d>5 threshold)\r\n");
        H.print();
        return false;
    }
    if (d < minWidth) {
        DBG("ERR:not calibrated (d>10 abs. threshold) %f\r\n", double(d));
        H.print();
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    //const int markwidth_percent = 5;
    const auto TH = m + (2 * d / 3);
    const auto TL = m + d / 3;
    const auto end = histogram::NBINS;
    const auto Q3 = H.count(2 * (end - 1) / 3, end);
    assert((end - 1) / 3 < end);
    const auto Q1 = H.count(0, (end - 1) / 3);
    const auto percent5 = 100 * float(Q1) / H.count();
    const auto percent95 = 100 * float(Q3) / H.count();
    if (std::fabs(percent5 - 5) > 1 || std::fabs(percent95 - 95) > 5) {
        DBG("ERR:not calibrated (percent5:%f percent95:%f)\r\n", double(percent5), double(percent95));
        H.print();
        return false;
    }
    status::instance.set(status::index::TH, TH);
    status::instance.set(status::index::TL, TL);
    *_TL = TL;
    *_TH = TH;
    return true;
}
//#include <math.h>
//static int t = 0;
const int espEnablePin = 3;
bool switchLED(bool on) {
    bool ret = false;

    static bool last_state = false;
    if (last_state != on) {
#ifdef ARDUINO
        digitalWrite(espEnablePin, on ? 1 : 0);
#endif
        ret = true;
    }
    last_state = on;
    return ret;
}

Detection::Detection() {
#ifdef ARDUINO
    pinMode(espEnablePin, OUTPUT);
#endif
    switchLED(false);
}

bool Detection::tick() {
    u16 value = 0;
    if (m_reader.done() && !m_reader.old()) {
        if (switchLED(false)) {
            value = round(m_reader.average());
        }
    }
    m_reader.tick();
    if (m_reader.old()) {
        m_reader.reset();
        assert(!m_reader.done());
        switchLED(true);
        // restart new measurement => nothing else to do.
        return false;
    }
    if (value == 0)
        return false;

    DBG("time:%d s analog value:%d\r\n", int(common::time::since_reset().value() / 1000), value);
    H.update(value);
    auto &TL = m_low_threshold;
    auto &TH = m_high_threshold;

    assert(TL <= TH);
    u16 TL2 = 0, TH2;

    auto iscalibrated = calibrated(H, &TL2, &TH2);
    if (iscalibrated) {
        TL = TL2;
        TH = TH2;
        DBG("update [%d]->[%d] [%d]->[%d]\r\n", TL, TL2, TH, TH2);
        H.reset();
    }

    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index >= size_adc) {
        memset((u8 *)m_last_adc_value, 0, sizeof(m_last_adc_value));
        m_adc_index = 0;
    }
    m_last_adc_value[m_adc_index] = value;
    m_adc_index++;

    status::instance.set(status::index::calibrated, u8(iscalibrated));
    //status::instance.dump();
    if (TL == TH) {
        DBG("ERR:not calibrated (count=%d)\r\n", H.count());
        return false;
    }
    DBG("TL=[%3d] TH=[%3d]\r\n", TL, TH);
    // is the value classificable ?
    if (TL < value && value < TH) {
        DBG("ERR out of range TL=[%3d] value=%d TH=[%3d]\r\n", TL, int(value), TH);
        return false;
    }
    const auto new_value = value >= TH;
    if (new_value == m_last_value)
        return false;
    m_last_value = new_value;
    if (new_value == 0) {
        return false;
    }
    DBG("TICK %d secs TL=[%3d] TH=[%3d]\r\n", int(common::time::since_reset().value() / 1000), TL, TH);
    return true;
}

const u8 *Detection::histogram_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    DBG("m_adc_index:%d size:%d\r\n", m_adc_index, int(size_adc));
    if (m_adc_index < size_adc)
        return 0;
    return (u8 *)H.get_packed(L);
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
histogram::Histogram make_histogram() {
    histogram::packed p;
    u32 k = 0;
    p.m_B1 = 100;
    //auto B={570, 725, 342, 42, 5, 37, 8};
    auto B = {142, 130, 187, 243, 180, 128, 62, 59, 10, 28, 12, 36, 20, 43, 14, 39};
    for (auto b : B) {
        p.bins[k++] = b;
    }
    p.m_B2 = p.m_B1 + sizeof(p.bins) / sizeof(p.bins[0]) - 1;
    return histogram::Histogram(p);
}

int Detection::test() {
    auto H = make_histogram();
    H.print();
    DBG("argmax=%d\n", int(H.argmax(111 + 1, 115)));
    //return 0;
    u16 TH = 0;
    u16 TL = 0;
    assert(TL <= TH);
    auto c = calibrated(H, &TL, &TH);
    DBG("c=%d\n", int(c));
    for (int k = 0; k < int(histogram::NBINS); ++k) {
        DBG("H[%d]=%d\n", int(H.min(k)), int(H.count(k)));
    }

    status::instance.dump();
    return 0;
}
#endif