#include "simulation.h"
#include "common/time.h"
#include "common/digital.h"
#include "common/debug.h"

#ifdef SIMULATION
bool simulation::tick() {
    static common::digital d3(3);
    static bool last_value = false;
    auto t = common::time::since_reset();
    auto seconds = t.value() / 1000;
    if (seconds == 0)
        return false;
    auto new_value = seconds % 4 == 0;
    auto fast = d3.read();
    debug::turnBuildinLED(fast);
    if (fast) {
        new_value = seconds % 2 == 0;
    }
    auto ret = last_value != new_value && new_value == true;
    last_value = new_value;
    return ret;
}
#else
bool simulation::tick() {
    assert(0);
}
#endif
