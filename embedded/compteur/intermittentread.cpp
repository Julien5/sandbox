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
    if (k < T) {
        auto a = m_analog->read();
        DBG("adcvalue:%d\r\n", int(a));
        A[k++] = a;
        last_measure_time = common::time::since_reset_us();
    }
    switchLED(!done());
    if (old()) {
        *value = round(average());
        reset();
        assert(!done());
        switchLED(true);
        return true;
    }
    return false;
}

bool IntermittentRead::done() const {
    return k == T;
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
    float ret = 0;
    const int N = (T - T0);
    for (int t = T0; t < T; ++t) {
        ret += float(A[t]) / N;
    }
    return ret;
}
