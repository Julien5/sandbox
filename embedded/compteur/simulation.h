#pragma once

#include "common/time.h"

#ifdef PC
namespace simulation {
    bool tick();
    common::time::ms end_time();
}
#endif
