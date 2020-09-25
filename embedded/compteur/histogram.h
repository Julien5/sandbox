#pragma once

#include "common/rusttypes.h"

namespace histogram {
    struct Bin {
        Bin();
        u16 value = 0;
        u32 count = 0;
    } __attribute__((packed));

    constexpr size_t NBINS = 20;

    struct packed {
        packed();
        Bin bins[NBINS];
    };

    class Histogram {
        packed m_packed;
        Bin *begin() const;
        Bin *end() const;
        void shrink_if_needed();

      public:
        void print() const;
        void update(u16 value);
        //! number of non-zero bins
        u16 size() const;
        //! number of update() calls.
        u32 count() const;
        u16 minimum() const;
        u16 maximum() const;
        /*! 
		  value K for which count(>K) > percent*count()
		  threshold(0) = maximum()
		  threshold(100) = minimum()
		*/
        u16 threshold(int percent) const;
        const packed *get_packed(size_t *L) const {
            *L = sizeof(m_packed);
            return &m_packed;
        }
        u32 count(u16 v) const;
        u16 argmax(u16 m, u16 M) const;
        u16 argmin(u16 m, u16 M) const;
        static int test();
    };
}
