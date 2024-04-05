#include "application.h"
#include "capacity.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/sleep.h"
#include "adcfile.h"
#include "compteur.h"
#include "httpsender.h"
#include "intermittentread.h"
#include "sleep_authorization.h"

std::unique_ptr<compteur> C;

namespace flags {
    bool last_transmit_failed = false;
}

std::unique_ptr<common::analog> m_analog;
#ifdef PC
std::unique_ptr<adcfile> adcFile;
#endif

static int n = 0;
const int N = 128;
struct Value {
    u16 adc = 0;
    common::time::ms time;
};
Value values[N];

void print_values() {
    for (int k = 0; k < N; ++k) {
        LOG("[%07lu] ticked:%03d\r\n", u32(values[k].time.value()), int(values[k].adc));
    }
}

void work() {
    if (n >= N) {
        n = 0;
        print_values();
        common::time::delay(common::time::ms(250));
        TRACE();
    }
    values[n].time = common::time::since_reset();
    values[n].adc = adcFile->read();
    n++;
}

void application::setup() {
    debug::init_serial();
    int q = 3;
    while (q--) {
        common::time::delay(common::time::ms(250));
        debug::turnBuildinLED(true);
        common::time::delay(common::time::ms(250));
        debug::turnBuildinLED(false);
    }
    m_analog = std::unique_ptr<common::analog>(new common::analog(0));
#ifdef PC
    if (!adcFile)
        adcFile = std::unique_ptr<adcfile>(new adcfile());
#endif
}

const common::time::ms sleeping_time(200);

void application::loop() {
    auto t0 = common::time::since_reset();
    sleep_authorization::reset();
    work();
    auto d = common::time::since_reset().since(t0);
    if (d > sleeping_time)
        return;
    assert(sleep_authorization::authorized());
    common::time::delay(sleeping_time.since(d));
    //sleep().deep_sleep(sleeping_time.since(d));
}
