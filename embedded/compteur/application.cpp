#include "application.h"
#include "common/debug.h"
#include "common/wifi.h"

#include "application.h"
#include "compteur.h"
#include "intermittentread.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<compteur> C;

void application::setup() {
    debug::init_serial();
    DBG("sizeof(compteur):%d\r\n", int(sizeof(compteur)));
    DBG("sizeof(wifi::wifi):%d\r\n", int(sizeof(wifi::wifi)));
    DBG("ok.%d\r\n", debug::freeMemory());
    C = std::unique_ptr<compteur>(new compteur);
    DBG("ok.%d\r\n", debug::freeMemory());
    W = std::unique_ptr<wifi::wifi>(new wifi::wifi);
}

class callback : public wifi::callback {
    int missing_bytes = -1;
    void status(u8 s) {
        DBG("status:%d\r\n", int(s));
        assert(s == 0);
    }
    void data_length(u16 total_length) {
        DBG("data_length:%d\r\n", int(total_length));
        missing_bytes = total_length;
    }
    virtual void data(u8 *data, size_t length) {
        missing_bytes -= length;
    }

  public:
    bool done() const {
        return missing_bytes == 0;
    }
};

class epoch_callback : public callback {
  public:
    u64 epoch = 0;
    void data(u8 *data, size_t length) {
        for (usize k = 0; k < length; ++k) {
            char n = data[k] - '0';
            auto ischar = 0 <= n && n <= 9;
            if (!ischar)
                continue;
            epoch = 10 * epoch + n;
        }
    }
};

u64 get_epoch() {
    TRACE();
    epoch_callback cb;
    W->get("http://pi:8000/epoch", &cb);
    if (cb.epoch < 1643485907) {
        DBG("invalid epoch:%ld\r\n", cb.epoch);
        return 0;
    }
    return cb.epoch;
}

bool transmit() {
    TRACE();
    auto t0 = common::time::since_reset();
    callback cb;
    size_t L = 0;
    auto data = C->data(&L);
    W->post("http://pi:8000/post", data, L, &cb);
    DBG("transmit time %d ms\r\n", int(common::time::since_reset().since(t0).value()));
    return cb.done();
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

bool need_transmit(const float &_current_power) {
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
        return common::time::ms(60 * 1000);
    }
}

bool hourly() {
    auto now = common::time::since_epoch();
    auto secs = now.value() / 1000;
    auto minutes = secs / 60;
    auto clockminutes = minutes % 60;
    return clockminutes == 0;
}

bool margin_since(const common::time::ms &_last_time) {
    auto now = common::time::since_epoch();
    return now.since(_last_time) > one_minute();
}

void update_epoch(bool force) {
    static common::time::ms last_time = common::time::ms(0);
    const auto time_has_come = hourly() && margin_since(last_time);
    if (last_time.value() == 0 || time_has_come || force) {
        auto e = get_epoch();
        if (e == 0)
            return;
        DBG("epoch:%d\r\n", int(e));
        common::time::set_current_epoch(common::time::ms(1000 * e));
        last_time = common::time::since_epoch();
    }
}

const bool force = true;
void application::loop() {
    update_epoch(!force);
    if (C->update()) {
        C->print();
        const auto P = last_known_power();
        if (need_transmit(P)) {
            if (transmit()) {
                transmitted_power = P;
                DBG("p1:%d -> p2:%d\r\n", int(transmitted_power), int(P));
                C->clear();
                update_epoch(force);
            } else
                assert(0);
            DBG("counter: %dW\n", int(P));
        }
    }
}
