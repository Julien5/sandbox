#include "application.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"
#include "common/sleep.h"

#include "application.h"
#include "compteur.h"
#include "status.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<compteur> C;

const int espEnablePin = 3;
std::unique_ptr<common::analog> analog;

void switchLED(bool on) {
    static bool last_state = false;
    if (last_state != on) {
#ifdef ARDUINO
        digitalWrite(espEnablePin, on ? 1 : 0);
#endif
    }
    last_state = on;
}

void application::setup() {
    debug::init_serial();
    DBG(".");
#ifdef ARDUINO
    pinMode(espEnablePin, OUTPUT);
    analog = std::unique_ptr<common::analog>(new common::analog());
#endif
    switchLED(false);
}

const int T0 = 4;
const int T = 10;

class IntermittentRead {
    size_t k = 0;
    u64 last_measure_time = 0;
    int A[T] = {0};

  public:
    IntermittentRead(){};
    u64 micros_since_last_measure() const {
        return common::time::micros_since_reset() - last_measure_time;
    }
    void tick() {
        if (k < T) {
            A[k++] = analog->read();
            last_measure_time = common::time::micros_since_reset();
        }
    }
    bool done() const {
        return k == T;
    }
    bool old() const {
        const auto age_ms = (common::time::micros_since_reset() - last_measure_time) / 1000;
        return age_ms > 200;
    }
    void reset() {
        for (int i = 0; i < T; ++i)
            A[i] = 0;
        k = 0;
        last_measure_time = 0;
    }
    int value(const size_t k) {
        return A[k];
    }
    float average() const {
        float ret = 0;
        const int N = (T - T0);
        for (int t = T0; t < T; ++t) {
            ret += float(A[t]) / N;
        }
        return ret;
    }
};

IntermittentRead A;
u64 start_on = 0;
u64 stop_on = 0;

void application::loop() {
    if (A.done() && !A.old()) {
        switchLED(false);
        if (stop_on == 0) {
            stop_on = common::time::micros_since_reset();
        }
    }
    A.tick();
    if (A.old()) {
        DBG("time on:%d\r\n", int(stop_on - start_on));
        //DBG("time:%d s analog value:%d\r\n", int(0), int(A.average()));
        for (size_t k = 0; k < T; ++k)
            DBG("%d ", int(A.value(k)));
        DBG("\r\n");
        A.reset();
        assert(!A.done());
        switchLED(true);
        start_on = common::time::micros_since_reset();
        stop_on = 0;
    }
}
