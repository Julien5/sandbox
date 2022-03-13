#include "intermittentread.h"
#include "adcfile.h"
#include "common/debug.h"
#include "common/time.h"
#include "sleep_authorization.h"
#include "alarmclock.h"

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
    assert(k < T);
    auto a = m_analog->read();
    A[k] = a;
    if (k == T - 1) {
        auto ambientlight = A[T - 1];
        *value = round(xMax(average() - ambientlight, 0.0f));
        reset();
        return true;
    }
    assert(k < (T - 1));
    sleep_authorization::forbid();
    return false;
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
