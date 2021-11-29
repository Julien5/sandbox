#pragma once

#include "common/rusttypes.h"

namespace common {
    namespace time {
        // set with an external time source.
        void set_current_epoch(u64 ms);
        u64 since_epoch();
        u32 since_reset();
        u64 micros_since_reset();
        u32 elapsed_since(const u32 t0);

        void delay(u32 d);
    };
};
