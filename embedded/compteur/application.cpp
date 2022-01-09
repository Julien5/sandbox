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
        DBG("receiving data at %p with length:%d\r\n", data, int(length));
        missing_bytes -= length;
        DBG("missing bytes:%d\r\n", missing_bytes);
    }

  public:
    bool done() const {
        return missing_bytes == 0;
    }
};

void transmit() {
    callback cb;
    size_t L = 0;
    auto data = C->data(&L);
    TRACE();
    W->post("https://httpbin.org/post", data, L, &cb);
    while (!cb.done()) {
        TRACE();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);
    }
}

bool need_transmit() {
    // C full || diff(rpm) > .25
    return true;
    if (C->is_full())
        return true;
    if (fabs(current_rpm - C->current_rpm()) > .25)
        return true;
    return false;
}

void application::loop() {
    if (C->update()) {
        C->print();
    }
    if (need_transmit()) {
        transmit();
    }
    current_rpm = C->current_rpm();
}
