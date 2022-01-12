#include "intermittentread.h"
#include "adcfile.h"
#include "common/debug.h"

const int espEnablePin = 3;
bool switchLED(bool on) {
    bool ret = false;

    static bool last_state = false;
    if (last_state != on) {
#ifdef ARDUINO
        digitalWrite(espEnablePin, on ? 1 : 0);
#endif
        ret = true;
    }
    last_state = on;
    return ret;
}

constexpr int IntermittentRead::T;

IntermittentRead::IntermittentRead() {
#ifdef ARDUINO
    pinMode(espEnablePin, OUTPUT);
#endif
    switchLED(false);

#ifdef PC
    common::analog_read_callback::install(adcfile::instance());
    adcfile::instance()->setT(T);
#endif
    m_analog = std::unique_ptr<common::analog>(new common::analog());
};

common::time::us IntermittentRead::micros_since_last_measure() const {
    return common::time::us(common::time::since_reset_us().value() - last_measure_time.value());
}

bool IntermittentRead::tick(u16 *value) {
    // first adc measure is thrown away, dont use light to save energy
    // last adc measure ambient light
    // hopefully swithing the LED off just before the measure does
    // not make the adc unstable
    switchLED(0 < 0 && k < (T - 1));
    if (k < T) {
        auto a = m_analog->read();
        A[k++] = a;
        last_measure_time = common::time::since_reset_us();
    }

    if (old()) {
        auto ambientlight = A[T - 1];
        *value = round(xMax(average() - ambientlight, 0.0f));
        reset();
        switchLED(true);
        return true;
    }
    return false;
}

bool IntermittentRead::old() const {
    const auto age = common::time::since_reset_us().since(last_measure_time);
    // DBG("since reset:%d,age:%d\r\n", int(common::time::since_reset_us().value()), int(common::time::ms(age).value()));
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
    const int N = (T - 1) - T0;
    for (int t = T0; t < T - 1; ++t) {
        ret += A[t];
    }
    return float(ret) / N;
}
