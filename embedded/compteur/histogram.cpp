#include "histogram.h"
#include <stdlib.h>
#include <string.h>

namespace impl {
    using namespace histogram;
    int compare(const void *cmp1, const void *cmp2) {
        Bin a = *(Bin *)(cmp1);
        Bin b = *(Bin *)(cmp2);
        return b.value - a.value;
    }

    void sort(Bin *begin, Bin *end) {
        const auto size_array = end - begin;
        const auto size_element = sizeof(*begin);
        qsort(begin, size_array, size_element, compare);
    }

    // no stl in arduino.
    template <typename I, typename F>
    void for_each(I b, I e, F f) {
        for (I it = b; it != e; ++it)
            f(*it);
    }
}

histogram::Bin::Bin() {
    //DBG("CTOR: value:%d count:%d\r\n",value,count);
}

histogram::packed::packed() {
    memset(bins, 0, sizeof(bins));
}

histogram::Bin *
histogram::Histogram::begin() const {
    return const_cast<histogram::Bin *>(m_packed.bins);
}

histogram::Bin *
histogram::Histogram::end() const {
    Bin *it = begin();
    while (it->count != 0 && it < (m_packed.bins + NBINS))
        ++it;
    assert(it >= begin());
    return it;
}

uint16_t
histogram::Histogram::size() const {
    return end() - begin();
}

uint32_t
histogram::Histogram::count() const {
    uint32_t ret = 0;
    impl::for_each(begin(), end(), [&](const Bin &bin) {
        // DBG("count(): value:%d count:%d\r\n",bin.value,bin.count);
        ret += bin.count;
    });
    return ret;
}

void histogram::Histogram::print() const {
    DBG("values: ");
    for (size_t k = 0; k < size(); ++k)
        DBG("[%3d] ", int(m_packed.bins[k].value));
    DBG("\r\n");
    DBG("counts: ");
    for (size_t k = 0; k < size(); ++k) {
        DBG("[%3d] ", int(m_packed.bins[k].count));
    }
    DBG("\r\n");
}

uint16_t
histogram::Histogram::minimum() const {
    Bin b;
    impl::for_each(begin(), end(), [&](const Bin &bin) {
        b = bin;
    });
    return b.value;
}

uint16_t
histogram::Histogram::maximum() const {
    Bin b;
    impl::for_each(begin(), end(), [&](const Bin &bin) {
        if (b.value == 0)
            b = bin;
    });
    return b.value;
}

uint16_t
histogram::Histogram::argmax(uint16_t m, uint16_t M) const {
    Bin ret;
    assert(ret.value == 0 && ret.count == 0);
    impl::for_each(begin(), end(), [&](const Bin &bin) {
        if (m <= bin.value && bin.value <= M)
            if (ret.count < bin.count)
                ret = bin;
    });
    return ret.value;
}

uint16_t
histogram::Histogram::argmin(uint16_t m, uint16_t M) const {
    Bin ret;
    ret.count = count();
    ret.value = 0;
    for (auto v = m; v < M; ++v) {
        const auto c = count(v);
        if (c < ret.count) {
            ret.count = c;
            ret.value = v;
        }
    }
    return ret.value;
}

uint32_t
histogram::Histogram::count(uint16_t v) const {
    for (auto it = begin(); it != end(); ++it)
        if (it->value == v)
            return it->count;
    return 0;
}

uint16_t
histogram::Histogram::threshold(int percent) const {
    if (count() == 0)
        return 0;
    const size_t wanted_count = count() * percent / 100;
    size_t accumulated_count = 0;
    Bin b = m_packed.bins[0];
    impl::for_each(begin(), end(), [&](const Bin &bin) {
        if (accumulated_count < wanted_count) {
            accumulated_count += bin.count;
            b = bin;
        }
    });
    assert(minimum() <= b.value && b.value <= maximum());
    return b.value;
}

size_t
index(const histogram::packed &p, uint16_t value) {
    for (size_t k = 0; k < histogram::NBINS; ++k) {
        if (p.bins[k].value == value)
            return k;
    }
    return histogram::NBINS;
}

void histogram::Histogram::shrink_if_needed() {
    const uint16_t max = 0xffff / 2;
    if (count() <= max)
        return;
    DBG("shrinking...\n");
    for (size_t k = 0; k < histogram::NBINS; ++k)
        m_packed.bins[k].count /= 2;
}

void histogram::Histogram::update(uint16_t value) {
    // DBG("update:%u\r\n",value);
    if (size() == histogram::NBINS && index(m_packed, value) == NBINS) {
        // full and value not found => remove the least.
        Bin least;
        least.count = count();
        least.value = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (least.count > it->count)
                least = *it;
        }
        auto least_index = index(m_packed, least.value);
        // DBG("remove least:%u\r\n",least.value);
        assert(least_index < histogram::NBINS);
        m_packed.bins[least_index].value = 0;
        m_packed.bins[least_index].count = 0;
    }
    auto k = index(m_packed, value);
    if (k == histogram::NBINS) {
        // value not found => use first free index.
        k = size();
        assert(m_packed.bins[k].count == 0);
        assert(m_packed.bins[k].value == 0);
        m_packed.bins[k].value = value;
    }
    assert(m_packed.bins[k].value == value);
    m_packed.bins[k].count++;
    shrink_if_needed();
    impl::sort(m_packed.bins, m_packed.bins + NBINS);
}

#ifdef DEVHOST
#include <fstream>
#endif

int histogram::Histogram::test() {
    DBG("testing\n");
    {
        Histogram H;
        DBG("size:%d\n", H.size());
        assert(H.size() == 0);
        H.update(2);
        H.update(2);
        H.update(3);
        H.update(1);
        DBG("count:%d\n", int(H.count()));
        assert(H.count() == 4);
        assert(H.size() == 3);
        assert(H.minimum() == 1);
        assert(H.maximum() == 3);
        assert(H.threshold(10) == 3);
        assert(H.threshold(100) == 1);
    }
    {
        Histogram H;
        for (int k = 15; k != 0; --k)
            H.update(k % 10);
        assert(H.count() == 15);
        assert(H.size() == 10);
        assert(H.minimum() == 0);
        assert(H.maximum() == 9);
        assert(H.threshold(50) == 4);
#ifdef DEVHOST
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
