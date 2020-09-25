#include "common/clock.h"
#include "common/debug.h"
#include "common/platform.h"
#include "common/time.h"

namespace Clock {
    ms millis_since_start() {
        return common::Time::since_reset();
    }

    mn minutes_since_start() {
        return millis_since_start() / (1000L * 60);
    }

    int test() {
        assert(millis_since_start() == 0);
        common::Time::delay(1000);
        assert(millis_since_start() == 1000);
        return 0;
    }
}
