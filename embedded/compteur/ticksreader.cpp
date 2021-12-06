#include "ticksreader.h"
#include "common/debug.h"
#include "common/time.h"
#include "histogram.h"
#include "status.h"
#include <math.h>
#include <string.h>

u16 bound(u16 T, u16 k) {
    return T + k;
}

//#define SEARCH

bool calibrated(histogram::Histogram H, u16 *_TL, u16 *_TH) {
    //H.print();
    const u8 minWidth = 4;
    const auto M = H.maximum();
    const auto m = H.minimum();
    //assert(M != 0);
    status::instance.set(status::index::M, M);
    status::instance.set(status::index::m, m);
    status::instance.set(status::index::line, __LINE__);
    DBG("m:%d M:%d\n", int(m), int(M));
    if (M - m < minWidth)
        return false;
    status::instance.set(status::index::line, __LINE__);
    const auto T0 = (m + M) / 2; //H.threshold(40);
    status::instance.set(status::index::T0, T0);
    DBG("T0:%d\n", int(T0));
    if (T0 == m || T0 == M) {
        DBG("ERR:not calibrated (1)\r\n");
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    const auto v1 = H.argmax(m, T0 - 1);
#ifdef SEARCH
    int k = 2;
    auto v2 = H.argmax(bound(T0, k), M);
    while (v2 == bound(T0, k) && (T0 + k) < M) {
        k++;
        v2 = H.argmax(bound(T0, k), M);
    }
    DBG("k=%d T0+k=%d b2=%d\n", k, T0 + k, u16(0.3 * m + 0.7 * M));
#else
    auto v2 = H.argmax(T0, M);
#endif
    status::instance.set(status::index::v1, v1);
    status::instance.set(status::index::v2, v2);
    DBG("v1:%d v2:%d\n", int(v1), int(v2));
    if (v1 == v2) {
        DBG("ERR:not calibrated T0:%d v1:%d v2:%d (2) \r\n", int(T0), int(v1), int(v2));
        H.print();
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    const auto v = H.argmin(v1, v2);
    DBG("v:%d \n", int(v));
    status::instance.set(status::index::v, v);
    if (v == v1 || v == v2) {
        DBG("ERR:not calibrated #:%d v1:%d v2:%d v:%d (3) \r\n", int(H.count()), int(v1), int(v2), int(v));
        H.print();
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    const auto TH = std::min(v + 1, int(v2));
    const auto TL = std::max(v - 1, int(v1));
    DBG("TL:%d TH:%d\n", int(TL), int(TH));
    status::instance.set(status::index::TH, TH);
    status::instance.set(status::index::TL, TL);
    *_TL = TL;
    *_TH = TH;
    return true;
}
//#include <math.h>
//static int t = 0;

bool switchLED(bool on) {
    bool ret = false;
    const int espEnablePin = 3;
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

TicksReader::TicksReader() {
    switchLED(false);
}

bool TicksReader::tick() {
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
    H.update(int(value));
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index >= size_adc) {
        memset((u8 *)m_last_adc_value, 0, sizeof(m_last_adc_value));
        m_adc_index = 0;
    }
    m_last_adc_value[m_adc_index] = value;
    m_adc_index++;
    //H.print();
    u16 TH = 0;
    u16 TL = 0;
    assert(TL <= TH);
    auto c = calibrated(H, &TL, &TH);
    status::instance.set(status::index::calibrated, u8(c));
    //status::instance.dump();
    if (!c) {
        DBG("ERR:not calibrated\r\n");
        return false;
    }
    DBG("TL=[%3d] TH=[%3d]\r\n", TL, TH);
    // is the value classificable ?
    if (TL < value && value < TH) {
        DBG("ERR:out of range\r\n");
        H.print();
        return false;
    }
    const auto new_value = value >= TH;
    if (new_value == m_last_value)
        return false;
    m_last_value = new_value;
    if (new_value == 0) {
        return false;
    }
    DBG("TICK TL=[%3d] TH=[%3d]\r\n", TL, TH);
    return true;
}

const u8 *TicksReader::histogram_data(usize *L) const {
    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    DBG("m_adc_index:%d size:%d\r\n", m_adc_index, int(size_adc));
    if (m_adc_index < size_adc)
        return 0;
    return (u8 *)H.get_packed(L);
}

const u8 *TicksReader::adc_data(usize *L) const {
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
    p.m_min = 100;
    //auto B={570, 725, 342, 42, 5, 37, 8};
    auto B = {142, 130, 187, 243, 180, 128, 62, 59, 10, 28, 12, 36, 20, 43, 14, 39};
    for (auto b : B) {
        p.bins[k++] = b;
    }
    p.m_max = p.m_min + sizeof(p.bins) / sizeof(p.bins[0]) - 1;
    return histogram::Histogram(p);
}

int TicksReader::test() {
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
        DBG("H[%d]=%d\n", int(H.value(k)), int(H.count(k)));
    }

    status::instance.dump();
    return 0;
}
#endif
