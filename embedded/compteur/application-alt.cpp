#include "application.h"
#include "common/debug.h"
#include "httpsender.h"
#include "common/sleep.h"
#include "application.h"
#include "compteur.h"
#include "intermittentread.h"
#include "sleep_authorization.h"
#include "capacity.h"

std::unique_ptr<compteur> C;

namespace flags {
    bool last_transmit_failed = false;
}
static int kounter = 0;
void printtime() {
    static common::time::ms last_time;
    auto now = common::time::since_reset();
    auto delta = now.since(last_time);
    DBG("%02d:%05d:%04d\r\n", kounter++, int(now.value()), int(delta.value()));
    last_time = now;
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
    //httpsender sender;
    //bool ok = sender.post_tickcounter(data, L);
    bool ok = true;
    if (!ok)
        return false;
    /*
    if (!setup_epoch(&sender)) {
        assert(0);
        return false;
    }
	*/

    capacity::measure m;
    data = m.data(&L);
    //ok = sender.post_capacity(data, L);
    ok = true;
    if (!ok) {
        assert(0);
        return false;
    }
    flags::last_transmit_failed = false;
    return true;
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
    return remain.value() < 15000;
}

char need_transmit_worker(bool ticked) {
    const char no = 0;
    if (night())
        return no;

    // do not transmit while the LED is turned on.
    if (!sleep_authorization::authorized())
        return no;

    if (hourly()) {
        return 1;
    }

    if (flags::last_transmit_failed)
        return no;

    // epoch should be set asap
    if (!common::time::epoch_is_set())
        return 2;

    if (ticked) {
        if (large_delta()) {
            return 3;
        }
        if (full()) {
            return 4;
        }
    }
    return no;
}

bool need_transmit(bool ticked) {
    if (ticks_coming_soon())
        return false;
    char reason = need_transmit_worker(ticked);
    if (reason != 0) {
        LOG("[%07lu] transmit:%03d\r\n", u32(common::time::since_reset().value()), int(reason));
        return true;
    }
    return false;
}

void work() {
    C->update();
}
#include "common/debug.h"
void application::setup() {
    debug::init_serial();
    int q = 3;
    while (q--) {
        common::time::delay(common::time::ms(250));
        debug::turnBuildinLED(true);
        common::time::delay(common::time::ms(250));
        debug::turnBuildinLED(false);
    }
    C = std::unique_ptr<compteur>(new compteur);
}

const common::time::ms sleeping_time(200);

void application::loop() {
    auto t0 = common::time::since_reset();
    sleep_authorization::reset();
    work();
    auto d = common::time::since_reset().since(t0);
    if (d > sleeping_time)
        return;
    if (sleep_authorization::authorized()) {
        //common::time::delay(sleeping_time);
        sleep().deep_sleep(sleeping_time.since(d));
    }
}
