#pragma once
#include "common/time.h"

namespace alarmclock {
    void wakein(const common::time::ms &delta);
    void unset();
    void sleep();
};
