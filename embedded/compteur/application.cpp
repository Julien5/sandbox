#include "application.h"
#include "common/debug.h"
#include "httpsender.h"

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

bool transmit() {
    size_t L = 0;
    auto data = C->data(&L);
    return httpsender().post_tickcounter(data, L);
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

bool need_transmit(const float &_current_power) {
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

bool hourly() {
    static common::time::ms last_trigger_time(0);
    auto now = common::time::since_epoch();
    auto secs = now.value() / 1000;
    auto minutes = secs / 60;
    auto clockminutes = minutes % 60;
    auto trigger = clockminutes % 5 == 0;
    // DBG("%d %d %ld\r\n", secs, int(trigger), now.since(last_trigger_time).value());
    if ((now > common::time::ms(1) && last_trigger_time.value() == 0) || (trigger && now.since(last_trigger_time) > one_minute())) {
        last_trigger_time = now;
        DBG("trigger(%d) last:%ld\r\n", int(secs), last_trigger_time.value());
        return true;
    }
    return false;
}

void hourly_tasks(bool force) {
    if (hourly() || force) {
        return;
        last_transmit_failed = false;
        auto e = get_epoch();
        if (e == 0)
            return;
        DBG("epoch:%d\r\n", int(e));
        common::time::set_current_epoch(common::time::ms(1000 * e));
    }
}

const bool force = true;
void application::loop() {
    hourly_tasks(!force);
    if (C->update()) {
        C->print();
        const auto P = last_known_power();
        if (need_transmit(P)) {
            if (transmit()) {
                transmitted_power = P;
                DBG("p1:%d -> p2:%d\r\n", int(transmitted_power), int(P));
                C->clear();
                hourly_tasks(force);
            } else {
                last_transmit_failed = true;
            }
            DBG("counter: %dW\n", int(P));
        }
    }
}
