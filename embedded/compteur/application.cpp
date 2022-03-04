#include "application.h"
#include "common/debug.h"
#include "httpsender.h"
#include "common/sleep.h"
#include "application.h"
#include "compteur.h"
#include "intermittentread.h"
#include "sleep_authorization.h"

std::unique_ptr<compteur> C;

namespace flags {
    bool last_transmit_failed = false;
}

bool setup_epoch_worker(httpsender *sender) {
    u64 e = 0;
    if (sender->get_epoch(&e)) {
        common::time::set_current_epoch(common::time::ms(e * 1000));
        return true;
    }
    return false;
}

bool setup_epoch(httpsender *sender = nullptr) {
    if (sender)
        return setup_epoch_worker(sender);
    httpsender s2;
    return setup_epoch_worker(&s2);
}

bool transmit() {
    flags::last_transmit_failed = true;
    size_t L = 0;
    C->print();
    auto data = C->data(&L);
    httpsender sender;
    bool ok = sender.post_tickcounter(data, L);
    if (ok) {
        if (setup_epoch(&sender))
            flags::last_transmit_failed = false;
    } else {
        // transmitting failed
        // no retry for now (we transmit hourly, this is enough.)
    }
    return ok;
}

bool night() {
    auto secs = common::time::since_epoch().value() / 1000;
    auto hours = secs / 3600;
    auto clockhours = 1 + (hours % 24); // epoch is UTC time => UTC+1
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
    return delta_power > 200;
}

bool full() {
    return C->is_full();
}

u16 hours(const common::time::ms &t) {
    return floor((t.value() / 1000) / 3600);
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
    auto next = C->last_tick().add(C->current_period());
    if (C->last_tick().value() == 0)
        return false;
    if (C->current_period().value() == 0)
        return false;
    auto now = common::time::since_epoch();
    if (now > next) {
        return true;
    }
    auto remain = next.since(now);
    assert(now.value() <= next.value());
    return remain.value() < 10000;
}

bool need_transmit_worker(bool ticked) {
    if (night())
        return false;

    // do not transmit while the LED is turned on.
    if (!sleep_authorization::authorized())
        return false;

    if (hourly()) {
        DBG("- hourly\r\n");
        return true;
    }

    if (flags::last_transmit_failed)
        return false;

    // epoch should be set asap
    if (!common::time::epoch_is_set())
        return true;

    if (ticked) {
        if (large_delta()) {
            DBG("- large_delta\r\n");
            return true;
        }
        if (full()) {
            DBG("- full\r\n");
            return true;
        }
    }
    return false;
}

bool need_transmit(bool ticked) {
    if (ticks_coming_soon())
        return false;
    if (need_transmit_worker(ticked)) {
        return true;
    }
    return false;
}

void work() {
    bool ticked = C->update();
    if (need_transmit(ticked)) {
        if (transmit()) {
            C->clear();
        }
    }
}

void application::setup() {
    debug::init_serial();
    DBG("hello\r\n");
    C = std::unique_ptr<compteur>(new compteur);
    DBG("init:memory:%d:time:%d\r\n", debug::freeMemory(), int(common::time::since_reset().value()));
    // this is sane in setup() to test the connection
    // also needed by night().
    setup_epoch();
}

void application::loop() {
    auto t0 = common::time::since_reset();
    sleep_authorization::reset();
    work();
    auto d = common::time::since_reset().since(t0);
    if (d.value() > 300)
        return;
    if (sleep_authorization::authorized())
        sleep().deep_sleep(common::time::ms(300 - d.value()));
}
