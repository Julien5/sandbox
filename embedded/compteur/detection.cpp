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

bool calibrated(const histogram::Histogram &H, u16 *_TL, u16 *_TH) {
    const u8 minWidth = 20;
    const auto M = H.maximum();
    const auto m = H.minimum();
    const auto d = M - m;
    auto ms = int(common::time::since_reset().value());
    PLOT("bounds:%f:%d:%d\r\n", float(ms) / 1000, int(m), int(M));
    if (H.count() < 180)
        return false;

    // ???
    if (d < float(m) * (5.0 / 100)) {
        H.print();
        return false;
    }
    if (d < minWidth) {
        H.print();
        return false;
    }
    status::instance.set(status::index::line, __LINE__);
    //const int markwidth_percent = 5;
    const auto TH = m + d / 2;
    const auto TL = m + d / 4;
    const auto end = histogram::NBINS;
    const auto part = (end - 1) / 4;
    const auto Q3 = H.count(2 * part, end);
    const auto Q1 = H.count(0, part);
    const auto percent5 = 100 * float(Q1) / H.count();
    const auto percent95 = 100 * float(Q3) / H.count();
    PLOT("features:%f:%f:%f\r\n", float(ms) / 1000, percent5, percent95);
    const auto percentlow = 2.5;
    if (std::fabs(percent5 - percentlow) > 2 || std::fabs(percent95 - (100 - percentlow)) > 2) {
        H.print();
        return false;
    }
    status::instance.set(status::index::TH, TH);
    status::instance.set(status::index::TL, TL);
    *_TL = TL;
    *_TH = TH;
    return true;
}

Detection::Detection() {
}

bool far_outside(const histogram::Histogram &H, const u16 &value) {
    const auto m = H.minimum();
    const auto M = H.maximum();
    if (m <= value && value <= M)
        return false;
    const auto d = value > M ? fabs(value - M) : fabs(value - m);
    const auto W = M - m;
    return d > xMax(100, int(W / 3));
}

static float xalpha = -1;
bool Detection::tick() {
    u16 value = 0;
    if (!m_reader.tick(&value))
        return false;
    float delta = 0;
    const float alpha = 0.95;
    if (xalpha < 0)
        xalpha = value;
    else {
        delta = fabs(xalpha - value);
        xalpha = alpha * xalpha + (1 - alpha) * value;
    }
    auto ms = int(common::time::since_reset().value());
    PLOT("values:%f:%d:%f\r\n", float(ms) / 1000, int(value), xalpha);
    PLOT("delta:%f:%f\r\n", float(ms) / 1000, delta);

    constexpr auto size_adc = sizeof(m_last_adc_value) / sizeof(m_last_adc_value[0]);
    if (m_adc_index >= size_adc) {
        memset((u8 *)m_last_adc_value, 0, sizeof(m_last_adc_value));
        m_adc_index = 0;
    }
    m_last_adc_value[m_adc_index] = value;
    m_adc_index++;

    const auto new_value = value < xalpha && delta > 70;
    if (new_value == m_last_value)
        return false;
    m_last_value = new_value;
    if (new_value == 0) {
        return false;
    }
    PLOT("ticks:%f:%d\r\n", float(ms) / 1000, value);
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
