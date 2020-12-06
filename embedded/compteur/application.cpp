#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"

#include "application.h"
#include "compteur.h"
#include "status.h"

std::unique_ptr<wifi::wifi>
    W;
std::unique_ptr<compteur> C;

const int espEnablePin = 3;
void enableEsp(bool enable) {
#if defined(ARDUINO)
    digitalWrite(espEnablePin, enable ? 1 : 0);
#endif
}
void application::setup() {
    debug::init_serial();
#if defined(ARDUINO)
    pinMode(espEnablePin, OUTPUT);
#endif
    W = std::unique_ptr<wifi::wifi>(new wifi::wifi);
    C = std::unique_ptr<compteur>(new compteur);
    DBG("memory:%d\r\n", debug::freeMemory());
}

class wcallback : public wifi::callback {
    size_t missing_bytes = 0;
    void status(u8 s) {
        DBG("receiving status %d\r\n", int(s));
        DBG("memory:%d\r\n", debug::freeMemory());
    }
    void data_length(u16 total_length) {
        DBG("receiving total %d bytes\r\n", int(total_length));
        missing_bytes = total_length;
    }
    void data(u8 *data, size_t length) {
        missing_bytes -= length;
        DBG("length %d missing %d\r\n", int(length), int(missing_bytes));
    }
};

void gather_data() {
    C->update();
}

void on_error(const int code) {
    DBG("post result:%d\r\n", int(code)); // TODO error handling.
    if (code == 0)
        return;
    common::time::delay(500);
}

class LEDRAI {
  public:
    LEDRAI() {
        debug::turnBuildinLED(true);
    }
    ~LEDRAI() {
        debug::turnBuildinLED(false);
    }
};
#include "common/serial.h"
class EspStarter {
  public:
    EspStarter() {
        enableEsp(true);
        // some time for startup
        common::time::delay(1000);
#ifdef ARDUINO
        while (Serial.available()) {
            Serial.read();
            TRACE();
        }
#endif
    }
    ~EspStarter() {
        enableEsp(false);
        // to pass transient states in the adc circuit
        common::time::delay(100);
    }
};

void send_data() {
    if (!C->ticksReader()->adc_data())
        return;
    EspStarter espStarter;
    LEDRAI ledrai;
    wcallback cb;
    {
        usize L = 0;
        const auto *data = C->ticksReader()->adc_data(&L);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/adc", data, L, &cb);
            on_error(p);
        } else
            return;
    }

    if (C->total() > 0) {
        usize L = 0;
        const u8 *data = C->data(&L);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/compteur", data, L, &cb);
            on_error(p);
        }
    }

    {
        usize L = 0;
        const auto *data = C->ticksReader()->histogram_data(&L);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/histogram", data, L, &cb);
            on_error(p);
        }
    }

    {
        usize L = 0;
        const auto *data = status::instance.data(&L);
        DBG("data:%d %d", data[0], data[1]);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/status", data, L, &cb);
            on_error(p);
        }
    }
}

void application::loop() {
    u32 t0 = common::time::since_reset();
    //TRACE();
    gather_data();
    //TRACE();
    //send_data();
    u32 t1 = common::time::since_reset();
    if ((t1 - t0) < 100)
        common::time::delay(100 - (t1 - t0)); // adjusted to effectively get 100ms sampling rate
}
