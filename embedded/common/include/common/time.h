#pragma once

#include "common/rusttypes.h"

namespace common {
    namespace time {
        // set with an external time source.
        void set_current_epoch(u64 ms);
        u64 since_epoch();
        u32 since_reset();
        void delay(u32 d);
    };
};
