#pragma once
#include "common/rusttypes.h"

namespace status {
    namespace index {
        const usize calibrated = 0;
        const usize line = 1;
        const usize m = 2;
        const usize M = 3;
        const usize T0 = 4;
        const usize v1 = 5;
        const usize v2 = 6;
        const usize v = 7;
        const usize TH = 8;
        const usize TL = 9;
    };
    class Data {
        static Data get();

      public:
        u16 D[16];

      public:
        Data();

        const u8 *data(usize *L) const;
        u16 get(const usize &indx);
        void set(const usize &indx, const u16 &value);
        void dump();
    };
    extern Data instance;
};
