#include "histogram.h"
#include "common/debug.h"
#include "common/platform.h"
#include "common/rusttypes.h"
#include "common/time.h"
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

float histogram::packed::min(const usize &index) const {
    return m_B1 + delta() * index;
}

float histogram::packed::delta() const {
    assert(NBINS > 1);
    return (m_B2 - m_B1) / (NBINS - 1);
}

float histogram::packed::max(const usize &index) const {
    return min(index) + delta();
}

usize histogram::packed::index(const u16 &value) const {
    if (m_B1 == m_B2) {
        return 0;
    }
    return (value - m_B1) / delta();
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

histogram::packed::packed() : m_B1(0xffff), m_B2(0) {
    memset(bins, 0, sizeof(bins));
}

u32 histogram::Histogram::count() const {
    return count(0, NBINS);
}

u32 histogram::Histogram::count(u16 from, u16 to) const {
    u32 ret = 0;
    assert(from <= NBINS && to <= NBINS);
    assert(0 <= from && from <= to);
    impl::for_each(begin() + from, begin() + to, [&](const histogram::Bin &bin) {
        ret += bin;
    });
    return ret;
}

bool intersection_empty(const histogram::packed &p1, size_t k, const histogram::packed &p2, size_t l) {
    const auto m1 = p1.min(k);
    const auto M1 = p1.max(k);
    const auto m2 = p2.min(l);
    const auto M2 = p2.max(l);
    if (M2 < m1 || m2 > M1) {
        return true;
    }
    return false;
}

float intersection_width(const histogram::packed &p1, size_t k, const histogram::packed &p2, size_t l) {
    const auto m1 = p1.min(k);
    const auto M1 = p1.max(k);
    const auto m2 = p2.min(l);
    const auto M2 = p2.max(l);
    const auto b1 = m1 < m2;
    const auto b2 = M1 < M2;
    //DBG("k:%d l:%d m1:%d M1:%d m2:%d M2:%d\r\n", int(k), int(l), int(m1), int(M1), int(m2), int(M2));
    float min = m1, max = M1;
    if (b1 && b2) {
        min = m2;
        max = M1;
    } else if (!b1 && !b2) {
        min = m1;
        max = M2;
    } else if (b1 && !b2) {
        min = m2;
        max = M2;
    } else if (!b1 && b2) {
        min = m1;
        max = M1;
    }
    assert(min <= max);
    return max - min;
}

template <typename R>
R positive(R x) {
    if (x < 0)
        return 0;
    return x;
}

void histogram::Histogram::spread(const float &m2, const float &M2) {
    histogram::packed p2;
    p2.m_B1 = m2;
    p2.m_B2 = M2;
    if (count() == 0) {
        m_packed = p2;
        return;
    }

    const auto oldcount = count();
    const auto &p1(m_packed);

    if (p1.delta() == 0) {
        auto l = p2.index(minimum());
        p2.bins[l] = oldcount;
        m_packed = p2;
        return;
    }

    float values[NBINS] = {0};
    for (size_t k = 0; k < NBINS; ++k) {
        for (size_t l = 0; l < NBINS; ++l) {
            if (intersection_empty(p1, k, p2, l))
                continue;
            const float I = intersection_width(p1, k, p2, l);
            if (I > 0 && p1.bins[k] > 0) {
                // DBG("(k:%d,l:%d,I:%f)\t", int(k), int(l), I);
                // DBG("(p1.bins[k]:%d,I/delta:%f)\r\n", int(p1.bins[k]), I / p1.delta());
            }
            values[l] += I * p1.bins[k] / p1.delta();
        }
    }
    float epsilon = 0;
    size_t newcount = 0;
    float values_sum = 0;
    for (size_t l = 0; l < NBINS; ++l) {
        values_sum += values[l];
        p2.bins[l] = positive(std::round(values[l] - epsilon));
        //DBG("x[%d]=%3.1f eps=%2.2f => xp=round[%3.1f]=%d\n", int(l), values[l], epsilon, values[l] - epsilon, p2.bins[l]);
        epsilon = p2.bins[l] - values[l];
        //DBG("=> eps=%f \n", epsilon);
        newcount += p2.bins[l];
    }
    auto enlarged = m2 <= minimum() && maximum() <= M2;
    float dcount = std::fabs(float(oldcount) - float(newcount));
    auto good = !enlarged || dcount <= 2;
    if (!good) {
        //DBG("not good: enlarged:%d old:%d new:%d dcount=%f values-sum=%f\r\n", int(enlarged), int(oldcount), int(newcount), double(dcount), double(values_sum));
        print();
        printf("vals:");
        for (size_t l = 0; l < NBINS; ++l) {
            printf("%5.1f    |", double(values[l]));
        }
        printf("\n");
        m_packed = p2;
        print();
        assert(0);
    }
    m_packed = p2;
}

void histogram::Histogram::print() const {
#ifdef PC
    auto ms = int(common::time::since_reset().value());
    printf("HIST:%5.1f:", float(ms) / 1000);
    for (size_t k = 0; k < size(); ++k)
        printf("%4.1f %4.1f|", m_packed.min(k), m_packed.max(k));
    printf("\r\n");
    printf("HIST:     :");
    for (size_t k = 0; k < size(); ++k)
        printf("%8d   |", int(m_packed.count(k)));
    printf("\r\n");
#else
    return;
    DBG("hist:c:");
    for (size_t k = 0; k < size(); ++k)
        DBG("[%3d] ", int(m_packed.count(k)));
    DBG("\r\n");
    DBG("hist:m:");
    for (size_t k = 0; k < size(); ++k)
        DBG("[%3d] ", int(m_packed.min(k)));
    DBG("\r\n");
    return;
    DBG("counts: ");
    for (size_t k = 0; k < size(); ++k) {
        DBG("[%3d] ", int(m_packed.count(k)));
    }
    DBG("\r\n");
#endif
}

void histogram::Histogram::print_gnuplot_block(const common::time::ms &ms) const {
#ifdef PC
    const char *prefix = "gnuplot:histogram";
    for (size_t k = 0; k < size(); ++k)
        printf("%s:%d %3.1f %3.2f\n", prefix, int(ms.value() / 1000), m_packed.min(k), float(count(k)) / count());
    printf("%s\n", prefix);
#else
#endif
}

float histogram::Histogram::minimum() const {
    return m_packed.min(0);
}

float histogram::Histogram::maximum() const {
    return m_packed.max(NBINS - 1);
}

u16 histogram::Histogram::argmax(u16 m, u16 M) const {
    usize imax = NBINS;
    u32 max = 0;
    for (usize index = 0; index < size(); ++index) {
        if (m <= m_packed.min(index) && m_packed.min(index) <= M)
            if (m_packed.count(index) > max) {
                imax = index;
                max = m_packed.count(index);
            }
    }
    return m_packed.min(imax);
}

u16 histogram::Histogram::argmin(u16 m, u16 M) const {
    usize imin = NBINS;
    u32 min = count();
    for (usize index = 0; index < size(); ++index) {
        if (m <= m_packed.min(index) && m_packed.min(index) <= M)
            if (m_packed.count(index) < min) {
                min = m_packed.count(index);
                imin = index;
            }
    }
    return m_packed.min(imin);
}

float histogram::Histogram::min(u16 index) const {
    return m_packed.min(index);
}

u32 histogram::Histogram::count(u16 index) const {
    return m_packed.count(index);
}

u16 histogram::Histogram::arghigh(int percent) const {
    if (count() == 0)
        return 0;
    assert(size() > 0);
    const float wanted_count = count() * float(percent) / 100;
    isize Gn = 0;
    isize n = size() - 1;
    while (n >= 0) {
        Gn += m_packed.count(n);
        if (Gn >= wanted_count)
            break;
        n--;
    }
    assert(minimum() <= m_packed.min(n) && m_packed.min(n) <= maximum());
    return m_packed.min(n);
}

u16 histogram::Histogram::arglow(int percent) const {
    if (count() == 0)
        return 0;
    assert(size() > 0);
    const float wanted_count = count() * float(percent) / 100;
    isize Gn = 0;
    isize n = 0;
    while (n >= 0) {
        Gn += m_packed.count(n);
        if (Gn >= wanted_count)
            break;
        n++;
    }
    assert(minimum() <= m_packed.min(n) && m_packed.min(n) <= maximum());
    return m_packed.min(n);
}

void histogram::Histogram::shrink_if_needed() {
    const float alpha = 0.9;
    const u16 kmax = round(-1800 * log10(alpha) / (log10(36) * (1 - alpha)));
    if (count() <= kmax)
        return;
    DBG("shrinking...\r\n");
    for (usize k = 0; k < size(); ++k)
        m_packed.bins[k] = floor(alpha * m_packed.bins[k]);
}

void histogram::Histogram::reset() {
    memset(&m_packed, 0, sizeof(m_packed));
    assert(m_packed.m_B1 == 0);
    assert(m_packed.m_B2 == 0);
    assert(m_packed.bins[0] == 0);
}

void histogram::Histogram::clear() {
    memset(&m_packed.bins, 0, sizeof(m_packed.bins));
    for (size_t k = 0; k < NBINS; ++k)
        assert(m_packed.bins[k] == 0);
    assert(m_packed.m_B1 > 0);
    assert(m_packed.m_B2 > 0);
    assert(m_packed.m_B2 != m_packed.m_B1);
    m_packed.bins[0] = 1;
    m_packed.bins[NBINS - 1] = 1;
}

void histogram::Histogram::update(u16 value) {
    if (count() == 0) {
        m_packed.m_B1 = value;
        m_packed.m_B2 = value;
        m_packed.bins[0] = 1;
        assert(minimum() == value);
        return;
    }
    auto m = min(0);
    auto M = min(NBINS - 1);
    auto m2 = xMin(float(value), m);
    auto M2 = xMax(float(value), M);
    auto middle = float(M + m) / 2.0f;
    const float alpha = 0.999;
    if (m < value && value < middle) {
        m2 = alpha * m + (1 - alpha) * value;
    } else if (middle < value && value < M) {
        M2 = alpha * M + (1 - alpha) * value;
    }
    spread(m2, M2);
    const auto k = m_packed.index(value);
    assert(k < size());
    m_packed.bins[k]++;
    shrink_if_needed();
    print_gnuplot_block(common::time::since_reset());
}

#ifdef PC
#include <fstream>
#endif

int histogram::Histogram::test() {
    DBG("testing (1)\n");
    {
        Histogram H;
        assert(H.minimum() > H.maximum());
        H.update(0);
        assert(H.minimum() == 0);
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
        assert(H.arghigh(0) == H.maximum());
        assert(H.arghigh(100) == H.minimum());
        assert(H.arghigh(50) == 7);
        H.print();
        DBG("high(10):%d\n", H.arghigh(10));
        assert(H.arghigh(10) == 8);
    }

    DBG("testing (2)\n");
    {
        Histogram H;
        H.update(0);
        H.update(9);
        for (int k = 15; k != 0; --k)
            H.update(k % 10);
        H.update(6);
        H.print();
        TRACE();
        assert(H.count() == 18);
        TRACE();
        assert(H.minimum() == 0);
        TRACE();
        assert(H.maximum() == 9);
        TRACE();
        DBG("H.high(30):%d\n", int(H.arghigh(30)));
        assert(H.arghigh(30) == 6);
        assert(H.arglow(50) == 3);
        H.print();
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

    {
        Histogram H;
        H.update(0);
        H.update(9);
        for (int k = 15; k != 0; --k)
            H.update(k % 10);
        H.update(6);
    }

    {
        Histogram H;
        H.update(0);
        H.update(NBINS - 1);
        // delta = 1
        H.update(4);
        H.update(5);
        assert(H.count(0) == 1);
        assert(H.count(NBINS - 1) == 1);
        assert(H.count(4) == 1);
        assert(H.count(5) == 1);
        assert(H.count() == 4);
        H.print();
        H.update(10 * (NBINS - 1));
        // delta = 10
        H.print();
        assert(H.count() == 5);
        H.update(200);
        H.print();
        assert(H.count() == 6);
    }
    DBG("ALL GOOD\n");
    return 0;
}
