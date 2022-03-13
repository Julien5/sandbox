#include "intermittentread.h"
#include "adcfile.h"
#include "common/debug.h"
#include "common/time.h"
#include "sleep_authorization.h"

const int ledEnablePin = 8;
bool switchLED(bool on) {
    bool ret = false;
    static bool last_state = false;
    if (last_state != on) {
#ifdef ARDUINO
        digitalWrite(ledEnablePin, on ? 1 : 0);
        common::time::delay(common::time::ms(1));
#endif
        ret = true;
    }
    last_state = on;
    return ret;
}

constexpr int IntermittentRead::T;

IntermittentRead::IntermittentRead() {
#ifdef ARDUINO
    pinMode(ledEnablePin, OUTPUT);
#endif
    switchLED(false);

#ifdef PC
    common::analog_read_callback::install(adcfile::instance());
    adcfile::instance()->setT(T);
#endif
    m_analog = std::unique_ptr<common::analog>(new common::analog(0));
};

common::time::us IntermittentRead::micros_since_last_measure() const {
    return common::time::us(common::time::since_reset_us().value() - last_measure_time.value());
}

bool IntermittentRead::done() const {
    return k >= T;
}

struct incrementer {
    isize *m_addr = 0;
    incrementer(isize *addr) : m_addr(addr) {}
    ~incrementer() {
        *m_addr = (*m_addr) + 1;
    };
};

bool IntermittentRead::tick(u16 *value) {
    // first adc measure is thrown away, dont use light to save energy
    // last adc measure ambient light
    // hopefully swithing the LED off just before the measure does
    // not make the adc unstable
    incrementer inc(&k);
    switchLED(0 < k && k < (T - 1));
    if (old())
        reset();

    if (k < T) {
        auto a = m_analog->read();
        A[k] = a;
        if (k == 0)
            last_measure_time = common::time::since_reset_us();
        // no sleep until k==T
        sleep_authorization::forbid();
    }

    if (k == T) {
        auto ambientlight = A[T - 1];
        *value = round(xMax(average() - ambientlight, 0.0f));
        return true;
    }
    return false;
}

bool IntermittentRead::old() const {
    const auto age = common::time::since_reset_us().since(last_measure_time);
    //DBG("age:%d\r\n", int(common::time::ms(age).value()));
    return common::time::ms(age).value() > 200;
}

void IntermittentRead::reset() {
    for (int i = 0; i < T; ++i)
        A[i] = 0;
    k = 0;
}

int IntermittentRead::value(const size_t k) {
    return A[k];
}

float IntermittentRead::average() const {
    u16 ret = 0;
    const int N = (T - 1) - K0;
    for (int t = K0; t < T - 1; ++t) {
        ret += A[t];
    }
    return float(ret) / N;
}
