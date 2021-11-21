#include "application.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"

#include "application.h"
#include "compteur.h"
#include "status.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<compteur> C;

const int espEnablePin = 3;
std::unique_ptr<common::analog> analog;

void application::setup() {
    debug::init_serial();
    DBG(".");
#ifdef ARDUINO
    pinMode(espEnablePin, OUTPUT);
    analog = std::unique_ptr<common::analog>(new common::analog());
#endif
}

void switchLED(bool on) {
#ifdef ARDUINO
    digitalWrite(espEnablePin, on ? 1 : 0);
#endif
}

const int T0 = 5;
const int T = 10;

int A[T] = {0};

void application::loop() {
    for (int t = 0; t < 100; ++t) {
        if (t == 0)
            switchLED(true);
        if (t < T) {
            int a = analog->read();
            if (T0 <= t)
                A[t] = a;
        }
        if (t == T) {
            switchLED(false);
            for (int k = T0; k < T; ++k)
                DBG("time:%d s analog value:%d\r\n", int(k), int(A[k]));
        }

        common::time::delay(1);
    }
}
