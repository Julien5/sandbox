#include "alarmclock.h"
#include "common/sleep.h"
namespace {
    static common::time::ms m_waketime;
}

void alarmclock::wakein(const common::time::ms &delta) {
    m_waketime = common::time::since_reset().add(delta);
}
void alarmclock::unset() {
    m_waketime = common::time::ms();
}

void alarmclock::sleep() {
    auto now = common::time::since_reset();
    if (m_waketime > now) {
        auto duration = m_waketime.since(now);
        ::sleep().deep_sleep(duration);
    }
}
