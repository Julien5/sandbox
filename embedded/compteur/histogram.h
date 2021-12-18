#pragma once

#include "common/rusttypes.h"
#include "common/time.h"

namespace histogram {
    constexpr size_t NBINS = 16;

    typedef u32 Bin;

    struct packed {
        packed();
        Bin bins[NBINS];
        float m_B1;
        float m_B2;
        usize index(const u16 &value) const;
        float min(const usize &index) const;
        float max(const usize &index) const;
        float delta() const;
        u32 count(const usize &index) const;
    } __attribute__((packed));

    class Histogram {
        packed m_packed;
        Bin *begin() const;
        Bin *end() const;
        void shrink_if_needed();
        void spread(const float &m2, const float &M2);

      public:
        Histogram(){};
        Histogram(const packed &p) : m_packed(p){};
        void print() const;
        void print_gnuplot_block(const common::time::ms &ms) const;
        void update(u16 value, const bool adapt = false);
        usize size() const;
        //! number of update() calls - 1 (first sets min/max)
        u32 count() const;
        float minimum() const;
        float maximum() const;
        /*! 
		  arghigh(0) = maximum()
		  arghigh(100) = minimum()
		*/
        u16 arghigh(int percent) const;
        u16 arglow(int percent) const;

        u32 count(u16 from, u16 to) const;

        const packed *get_packed(size_t *L) const {
            *L = sizeof(m_packed);
            return &m_packed;
        }
        float min(u16 index) const;
        u32 count(u16 index) const;
        u16 argmax(u16 m, u16 M) const;
        u16 argmin(u16 m, u16 M) const;
        static int test();
    };
}
