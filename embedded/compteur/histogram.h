#pragma once

#include "common/rusttypes.h"

namespace histogram {
    constexpr size_t NBINS = 16;

    typedef u32 Bin;

    struct packed {
        packed();
        Bin bins[NBINS];
        float m_min;
        float m_max;
        usize index(const u16 &value) const;
        u16 value(const usize &index) const;
        u32 count(const usize &index) const;
    } __attribute__((packed));

    class Histogram {
        packed m_packed;
        Bin *begin() const;
        Bin *end() const;
        void shrink_if_needed();

      public:
        Histogram(){};
        Histogram(const packed &p) : m_packed(p){};
        void print() const;
        void update(u16 value);
        usize size() const;
        //! number of update() calls - 1 (first sets min/max)
        u32 count() const;
        u16 minimum() const;
        u16 maximum() const;
        /*! 
		  high(0) = maximum()
		  high(100) = minimum()
		*/
        u16 high(int percent) const;
        u16 low(int percent) const;
        const packed *get_packed(size_t *L) const {
            *L = sizeof(m_packed);
            return &m_packed;
        }
        u16 value(u16 index) const;
        u32 count(u16 index) const;
        u16 argmax(u16 m, u16 M) const;
        u16 argmin(u16 m, u16 M) const;
        static int test();
    };
}
