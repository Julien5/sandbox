#pragma once

#include "common/rusttypes.h"

namespace Clock {
    using ms = u32;
    using mn = u16;
    ms millis_since_start();
    mn minutes_since_start();
    int test();
}
