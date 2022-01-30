#include "compteur.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/sleep.h"

tickscounter::counter_config config() {
    tickscounter::counter_config ret;
    ret.kMinAloneTicks = 0; // disable denoising
    ret.kSecondsUntilAloneTick = 3;
    return ret;
}

compteur::compteur()
    : counter(config()) {
    debug::address_range("compteur:", this, sizeof(*this));
}

Detection *compteur::detection() {
    return &m_detection;
}

double kW(const u32 ticks, const Clock::ms d) {
    if (d == 0)
        return 0;
    const double hours = double(d) * 0.001 / 3600.0f;
    return (double(ticks) / hours) / 75.0f;
}

void print_bin(const tickscounter::bin &b) {
    // DBG("start:%7d duration:%7d count:%3d\n",b.m_start, b.m_duration,b.m_count);
}

bool compteur::update() {
    if (m_detection.tick()) {
        counter.tick();
        DBG("counter ticked\r\n");
        return true;
    }
    //DBG("counter not ticked\r\n");
    if (m_detection.may_sleep())
        sleep().deep_sleep(common::time::ms(200));
    return false;
}

void compteur::print() {
    counter.print();
    DBG("counter:%d:total:%d\r\n", int(common::time::since_reset().value()) / 1000, int(counter.total()));
}

const u8 *compteur::data(size_t *L) const {
    return reinterpret_cast<const u8 *>(counter.get_packed(L));
}

tickscounter::bin::count compteur::total() {
    return counter.total();
}

float compteur::current_rpm() {
    const auto N = counter.total();
    if (N < 2)
        return 0;
    const auto ms0 = common::time::ms(counter.getbin(0).m_start);
    const auto ms1 = common::time::ms(counter.last_tick_time());
    float minutes = float(ms1.since(ms0).value()) / (1000 * 60);
    return float(N) / minutes;
}

bool compteur::is_full() const {
    return counter.is_full();
}

void compteur::clear() {
    counter.reset();
}

int compteur::test() {
    return 0;
    /*
    compteur U;
    while (true) {
        if (U.update())
            U.print();
        common::time::delay(common::time::ms(200));
    }
	*/
}

common::time::ms compteur::time_between_last_two_ticks() const {
    if (counter.bin_count() < 2)
        return common::time::ms();
    common::time::ms t1, t2; // zero-init
    for (auto k = tickscounter::NTICKS - 1; k >= 0; --k) {
        if (counter.getbin(k).empty())
            continue;
        auto bin = counter.getbin(k);
        if (t1.value() == 0) {
            if (bin.m_count > 1) {
                return common::time::ms(bin.m_duration / (bin.m_count - 1));
            }
            t1 = common::time::ms(bin.start());
        } else {
            t2 = common::time::ms(bin.end());
            break;
        }
    }
    return common::time::ms(t1.value() - t2.value());
}
