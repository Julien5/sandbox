#include "application.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/serial.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"
#include "common/sleep.h"

#include "application.h"
#include "compteur.h"
#include "status.h"
#include "intermittentread.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<compteur> C;
float current_rpm = 0;

void application::setup() {
    debug::init_serial();
    DBG("sizeof(compteur):%d\r\n", int(sizeof(compteur)));
    DBG("sizeof(wifi::wifi):%d\r\n", int(sizeof(wifi::wifi)));
    DBG("sizeof(common::serial):%d\r\n", int(sizeof(common::serial)));
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
    void data(u8 *data, size_t length) {
        missing_bytes -= length;
    }

  public:
    bool done() const {
        return missing_bytes == 0;
    }
};

void transmit() {
    TRACE();
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    return;
    callback cb;
    size_t L = 0;
    auto data = C->data(&L);
    TRACE();
    W->post("https://httpbin.org/post", data, L, &cb);
    // todo: timeout
    while (!cb.done()) {
        TRACE();

        std::this_thread::sleep_for(1000ms);
    }
}

bool need_transmit() {
    // C full || diff(rpm) > .25
    if (C->is_full())
        return true;
    auto rpm = C->current_rpm();
    if (rpm == 0)
        return false;
    return fabs(current_rpm - rpm) > .25;
}

void application::loop() {
    if (C->update()) {
        C->print();
    }
    if (need_transmit()) {
        transmit();
        DBG("counter: :%3.1f -> %3.1f\r\n", float(current_rpm), float(C->current_rpm()));
        current_rpm = C->current_rpm();
        C->clear();
    }
}
