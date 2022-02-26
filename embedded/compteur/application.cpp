#include "application.h"
#include "common/debug.h"
#include "httpsender.h"
#include "common/sleep.h"
#include "application.h"
#include "compteur.h"
#include "intermittentread.h"

std::unique_ptr<compteur> C;

namespace flags {
    bool need_transmit = false;
}

bool transmit() {
    assert(flags::need_transmit);
    size_t L = 0;
    C->print();
    auto data = C->data(&L);
    bool ok = httpsender().post_tickcounter(data, L);
    assert(ok);
    // if (ok) // no retry for now.
    flags::need_transmit = false;
    return ok;
}

bool night() {
    auto secs = common::time::since_epoch().value() / 1000;
    auto hours = secs / 3600;
    auto clockhours = 1 + (hours % 24); // epoch is UTC time => UTC+1
    //DBG("clockhours:%d\r\n", int(clockhours));
    return 23 <= clockhours || clockhours < 6;
}

float power(const common::time::ms &interval) {
    if (interval.value() == 0)
        return 0;
    const int K = 75;
    const float T = float(interval.value()) / 1000;
    return 1000 * float(3600) / (T * K);
}

bool large_delta() {
    auto T0 = double(C->previous_period().value()) / 1000;
    auto T1 = double(C->current_period().value()) / 1000;
    if (T0 * T1 == 0)
        return false;
    auto delta_power = (double(1000) * 3600 / 75) * fabs(T1 - T0) / (T1 * T0);
    DBG("T0:%2.2f s T1:%2.2f\r\n", T0, T1);
    DBG("delta:%2.2f s power:%2.2f W\r\n", fabs(T0 - T1), delta_power);
    return delta_power > 200;
}

bool full() {
    return C->is_full();
}

u16 hours(const common::time::ms &t) {
    return std::floor((t.value() / 1000) / 3600);
}

bool hourly() {
    static common::time::ms last_trigger_time(0);
    auto now = common::time::since_epoch();
    if (hours(now) != hours(last_trigger_time)) {
        bool first_time = last_trigger_time.value() == 0;
        last_trigger_time = now;
        if (first_time)
            return false;
        return true;
    }
    return false;
}

bool ticks_coming_soon() {
    //DBG("soon:\r\n");
    auto next = C->last_tick().add(C->current_period());
    if (C->last_tick().value() == 0)
        return false;
    //DBG("soon:last:%zu\r\n", C->last_tick().value());
    if (C->current_period().value() == 0)
        return false;
    //DBG("soon:   T:%zu\r\n", C->current_period().value());
    auto now = common::time::since_epoch();
    //DBG("soon: now:%zu\r\n", now.value());
    //DBG("soon:next:%zu\r\n", next.value());
    if (now > next) {
        return true;
    }
    auto remain = next.since(now);
    //DBG("soon:%zu\r\n", remain.value());
    assert(now.value() <= next.value());
    return remain.value() < 10000;
}

bool need_transmit_worker(bool ticked) {
    if (night()) {
        return false;
    }
    if (ticked) {
        if (large_delta()) {
            DBG("\r\n");
            return true;
        }
        if (full()) {
            DBG("\r\n");
            return true;
        }
    }
    if (hourly()) {
        DBG("\r\n");
        return true;
    }
    return false;
}

bool need_transmit(bool ticked) {
    if (flags::need_transmit)
        return true;
    if (need_transmit_worker(ticked)) {
        flags::need_transmit = true;
        return flags::need_transmit;
    }
    return false;
}

bool setup_epoch() {
    TRACE();
    u64 e = 0;
    if (httpsender().get_epoch(&e)) {
        common::time::set_current_epoch(common::time::ms(e * 1000));
        return true;
    }
    return false;
}

namespace {
    common::time::ms one_minute() {
        return common::time::ms(u64(60) * 1000);
    }
}

void work() {
    bool ticked = C->update();
    if (ticks_coming_soon())
        return;
    if (need_transmit(ticked)) {
        if (transmit()) {
            C->clear();
            setup_epoch();
        }
    }
}

void application::setup() {
    debug::init_serial();
    C = std::unique_ptr<compteur>(new compteur);
    DBG("ok.%d\r\n", debug::freeMemory());
    while (!setup_epoch()) {
        DBG("failed\r\n");
    }
}

void application::loop() {
    auto t0 = common::time::since_reset();
    work();
    auto d = common::time::since_reset().since(t0);
    if (d.value() > 300)
        return;
    sleep().deep_sleep(common::time::ms(300 - d.value()));
}
