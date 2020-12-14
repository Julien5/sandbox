#include "histogram.h"
#include "common/debug.h"
#include "common/platform.h"
#include "common/rusttypes.h"
#include <stdlib.h>
#include <string.h>

namespace impl {
    using namespace histogram;
    // no stl in arduino.
    template <typename I, typename F>
    void for_each(I b, I e, F f) {
        for (I it = b; it != e; ++it)
            f(*it);
    }
}

u16 histogram::packed::value(const usize &index) const {
    return min + (max - min) * float(index) / (NBINS - 1);
}

usize histogram::packed::index(const u16 &value) const {
    if (min == max) {
        return 0;
    }
    return (NBINS - 1) * (value - min) / (max - min);
}

u32 histogram::packed::count(const usize &index) const {
    return bins[index];
}

histogram::Bin *histogram::Histogram::begin() const {
    return const_cast<histogram::Bin *>(m_packed.bins);
}

histogram::Bin *histogram::Histogram::end() const {
    return const_cast<histogram::Bin *>(m_packed.bins + NBINS);
}

usize histogram::Histogram::size() const {
    return end() - begin();
}

histogram::packed::packed() : min(0xffff), max(0) {
    DBG("min:%d\r\n", int(min));
    memset(bins, 0, sizeof(bins));
}

u32 histogram::Histogram::count() const {
    u32 ret = 0;
    impl::for_each(begin(), end(), [&](const histogram::Bin &bin) {
        ret += bin;
    });
    return ret;
}

void histogram::Histogram::print() const {
#ifdef PC
    for (size_t k = 0; k < size(); ++k)
        DBG("%3d:H[%3d]=%3d\n", int(k), int(m_packed.value(k)), int(m_packed.count(k)));
#else
    DBG("values: ");
    for (size_t k = 0; k < size(); ++k)
        DBG("[%3d] ", int(m_packed.value(k)));
    DBG("\r\n");
    DBG("counts: ");
    for (size_t k = 0; k < size(); ++k) {
        DBG("[%3d] ", int(m_packed.count(k)));
    }
    DBG("\r\n");
#endif
}

u16 histogram::Histogram::minimum() const {
    return m_packed.min;
}

u16 histogram::Histogram::maximum() const {
    return m_packed.max;
}

u16 histogram::Histogram::argmax(u16 m, u16 M) const {
    Bin ret = 0;
    for (usize k = 0; k < size(); ++k) {
        if (m <= m_packed.value(k) && m_packed.value(k) <= M)
            if (m_packed.count(k) > ret)
                ret = m_packed.value(k);
    }
    return ret;
}

u16 histogram::Histogram::argmin(u16 m, u16 M) const {
    Bin ret = count();
    for (usize k = 0; k < size(); ++k) {
        if (m <= m_packed.value(k) && m_packed.value(k) <= M)
            if (m_packed.count(k) < ret)
                ret = m_packed.value(k);
    }
    return ret;
}

u32 histogram::Histogram::count(u16 v) const {
    return m_packed.value(v);
}

u16 histogram::Histogram::threshold(int percent) const {
    TRACE();
    if (count() == 0)
        return 0;
    assert(size() > 0);
    TRACE();

    const float wanted_count = count() * float(percent) / 100;
    TRACE();
    DBG("percent:%d,wanted:%f\r\n", percent, wanted_count);
    isize Gn = 0;
    isize n = size() - 1;
    /*
    for (index = size() - 1; (index >= 0) && (float(accumulated_count) < wanted_count); index--) {

        DBG("acc:%d,index=%d cond=%d\r\n", int(accumulated_count), index, float(accumulated_count) < wanted_count);
		}*/
    DBG("acc:%d,wanted=%d\r\n", int(Gn), int(wanted_count));
    while (n >= 0) {
        Gn += m_packed.count(n);
        DBG("G(%d):%d wanted:%d\r\n", n, int(Gn), int(wanted_count));
        if (Gn >= wanted_count)
            break;
        n--;
    }
    DBG("G(%d):%d\r\n", n, int(Gn));
    assert(minimum() <= m_packed.value(n) && m_packed.value(n) <= maximum());
    return m_packed.value(n);
}

void histogram::Histogram::shrink_if_needed() {
    const u16 max = 0xffff / 2;
    if (count() <= max)
        return;
    DBG("shrinking...\n");
    for (usize k = 0; k < size(); ++k)
        m_packed.bins[k] /= 2;
}

void histogram::Histogram::update(u16 value) {
    DBG("update:%u\r\n", value);
    // fixme
    m_packed.min = xMin(value, m_packed.min);
    m_packed.max = xMax(value, m_packed.max);
    const usize k = m_packed.index(value);
    if (k == size())
        return;
    m_packed.bins[k]++;
    shrink_if_needed();
}

#ifdef PC
#include <fstream>
#endif

int histogram::Histogram::test() {
    DBG("testing (1)\n");
    {
        Histogram H;
        H.update(0);
        assert(H.minimum() == 0);
        assert(H.maximum() == 0);
        assert(H.count() == 1);

        H.update(9);
        assert(H.minimum() == 0);
        assert(H.maximum() == 9);
        assert(H.count() == 2);

        H.update(2);
        H.update(2);
        H.update(2);
        H.update(2);
        H.update(8);
        H.update(8);
        H.update(8);
        H.update(8);

        H.print();

        DBG("count:%d\n", int(H.count()));
        assert(H.count() == 10);
        assert(H.minimum() == 0);
        assert(H.maximum() == 9);
        assert(H.threshold(0) == H.maximum());
        assert(H.threshold(100) == H.minimum());
        assert(H.threshold(50) == 8);
        assert(H.threshold(10) == 9);
    }

    DBG("testing (2)\n");
    {
        Histogram H;
        H.update(0);
        H.update(9);
        for (int k = 15; k != 0; --k)
            H.update(k % 10);
        H.print();
        TRACE();
        assert(H.count() == 17);
        TRACE();
        assert(H.minimum() == 0);
        TRACE();
        assert(H.maximum() == 9);
        TRACE();
        assert(H.threshold(50) == 4);
        TRACE();
#ifdef PC
        size_t L = 0;
        const packed *data = H.get_packed(&L);
        using namespace std;
        std::ofstream file;
        file.open("histogram.packed.bin", ios::out | ios::binary);
        file.write((char *)data, L);
#endif
    }
    DBG("ALL GOOD\n");
    return 0;
}
