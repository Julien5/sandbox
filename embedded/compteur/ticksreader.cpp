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
        DBG("ERR:not calibrated (d>5% threshold)\r\n");
        H.print();
        return false;
    }
    if (d < minWidth) {
        DBG("ERR:not calibrated (d>10 abs. threshold)\r\n");
        H.print();
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    const auto TH = H.high(70);
    const auto TL = H.low(100 / 20);
    if (TH <= TL) {
        DBG("ERR:not calibrated #:%d TL:%d TH:%d (3) \r\n", int(H.count()), int(TH), int(TL));
        H.print();
        return false;
    }
    const auto dT = TH - TL;
    if (3 * dT < d) {
        DBG("ERR:not calibrated (dT<d/3 threshold #:%d TL:%d TH:%d (3) \r\n", int(H.count()), int(TL), int(TH));
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
        //DBG("ERR out of range TL=[%3d] value=%d TH=[%3d]\r\n", TL, int(value), TH);
        // H.print();
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
