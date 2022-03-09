#pragma once
#include "common/rusttypes.h"

namespace capacity {
    struct packed {
        u16 c1 = 0;
        u16 c2 = 0;
    } __attribute__((packed));

    class measure {
        packed m_packed;

      public:
        measure();
        ~measure();
        const u8 *data(usize *L);
    };
}
