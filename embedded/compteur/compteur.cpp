#include "compteur.h"
#include "common/debug.h"
#include "common/time.h"

tickscounter::counter_config config() {
    tickscounter::counter_config ret;
    ret.kMinAloneTicks = 0; // disable denoising
    ret.kSecondsUntilAloneTick = 3;
    return ret;
}

compteur::compteur()
    : m_counter(config()) {
    memset(m_last_ticks, 0, sizeof(m_last_ticks));
}

Detection *compteur::detection() {
    return &m_detection;
}

void print_bin(const tickscounter::bin &b) {
    // DBG("start:%7d duration:%7d count:%3d\n",b.m_start, b.m_duration,b.m_count);
}

void push(common::time::ms *values, const usize N, const common::time::ms &value) {
    for (usize k = 0; k < (N - 1); ++k) {
        values[k] = values[k + 1];
    }
    values[N - 1] = value;
}

bool compteur::update() {
    if (m_detection.tick()) {
        m_counter.tick();
        push(m_last_ticks, sizeof(m_last_ticks) / sizeof(m_last_ticks[0]), common::time::since_epoch());
        //DBG("counter ticked\r\n");
        return true;
    }
    return false;
}

void compteur::print() {
    m_counter.print();
    DBG("counter:%d:total:%d\r\n", int(common::time::since_reset().value()) / 1000, int(m_counter.total()));
}

const u8 *compteur::data(size_t *L) const {
    return reinterpret_cast<const u8 *>(m_counter.get_packed(L));
}

tickscounter::bin::count compteur::total() {
    return m_counter.total();
}

common::time::ms compteur::current_period() const {
    return m_last_ticks[2].since(m_last_ticks[1]);
}

common::time::ms compteur::delta_period() const {
    const auto d1 = m_last_ticks[1].since(m_last_ticks[0]);
    const auto d2 = m_last_ticks[2].since(m_last_ticks[1]);
    if (d1.value() == 0 || d2.value() == 0)
        return common::time::ms();
    return common::time::ms(fabs(double(d1.value()) - double(d2.value())));
}

common::time::ms compteur::last_tick() const {
    for (int k = 2; k >= 0; --k) {
        if (m_last_ticks[k].value() > 0)
            return m_last_ticks[k];
    }
    return common::time::ms(0);
}

bool compteur::is_full() const {
    return m_counter.is_full();
}

void compteur::clear() {
    m_counter.reset();
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
