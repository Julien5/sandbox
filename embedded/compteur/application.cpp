#include "application.h"
#include "common/debug.h"
#include "httpsender.h"
#include "common/sleep.h"
#include "application.h"
#include "compteur.h"
#include "intermittentread.h"

std::unique_ptr<compteur> C;

void application::setup() {
    debug::init_serial();
    DBG("sizeof(compteur):%d\r\n", int(sizeof(compteur)));
    DBG("sizeof(wifi::wifi):%d\r\n", int(sizeof(httpsender)));
    DBG("ok.%d\r\n", debug::freeMemory());
    C = std::unique_ptr<compteur>(new compteur);
    DBG("ok.%d\r\n", debug::freeMemory());
}

namespace flags {
    bool need_transmit = false;
}

void transmit() {
    assert(flags::need_transmit);
    size_t L = 0;
    auto data = C->data(&L);
    bool ok = httpsender().post_tickcounter(data, L);
    // if (ok) // no retry for now.
    flags::need_transmit = false;
}

bool night() {
    auto secs = common::time::since_epoch().value() / 1000;
    auto hours = secs / 3600;
    auto clockhours = 1 + (hours % 24); // epoch is UTC time.
    return 23 <= clockhours || clockhours < 6;
}

bool large_delta() {
    return C->delta() > 200;
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
        last_trigger_time = now;
        return true;
    }
    return false;
}

bool ticks_coming_soon() {
    // use compteur
    return false;
}

bool need_transmit_worker(bool ticked) {
    if (night())
        return false;
    if (ticked) {
        if (large_delta())
            return true;
        if (full())
            return true;
    }
    if (hourly())
        return true;
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

u64 get_epoch() {
    TRACE();
    u64 e = 0;
    if (httpsender().get_epoch(&e))
        return e;
    return 0;
}

float power(const common::time::ms &interval) {
    if (interval.value() == 0)
        return 0;
    const int K = 70;
    const float T = float(interval.value()) / 1000;
    return 1000 * float(3600) / (T * K);
}

float last_known_power() {
    const auto T = C->time_between_last_two_ticks();
    DBG("p1 T:%d\r\n", int(T.value()));
    return power(T);
}

float transmitted_power = 0;
bool last_transmit_failed = false;

bool need_transmit_0(const float &_current_power) {
    if (last_transmit_failed)
        return false;
    // C full || diff(rpm) > 200W
    if (C->is_full()) {
        TRACE();
        return true;
    }
    if (_current_power == 0)
        return false;
    const auto delta = transmitted_power - _current_power;
    if (fabs(delta) > 200) {
        DBG("p1:%d -> p2:%d\r\n", int(transmitted_power), int(_current_power));
        TRACE();
        return true;
    }
    return false;
}

namespace {
    common::time::ms one_minute() {
        return common::time::ms(u64(60) * 1000);
    }
}

void hourly_tasks(bool force) {
    if (hourly() || force) {
        last_transmit_failed = false;
        auto e = get_epoch();
        if (e == 0)
            return;
        DBG("epoch:%d\r\n", int(e));
        common::time::set_current_epoch(common::time::ms(1000 * e));
    }
}

void work() {
    bool ticked = C->update();
    if (ticks_coming_soon())
        return;
    if (need_transmit(ticked)) {
        transmit();
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
