#include "application.h"
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

void application::setup() {
    debug::init_serial();
    W = std::unique_ptr<wifi::wifi>(new wifi::wifi);
    C = std::unique_ptr<compteur>(new compteur);
    DBG("memory:%d\r\n", debug::freeMemory());
}

class wcallback : public wifi::callback {
    size_t missing_bytes = 0;
    void status(u8 s) {
        DBG("receiving status %d\r\n", int(s));
        //assert(s == 0);
        DBG("memory:%d\r\n", debug::freeMemory());
    }
    void data_length(u16 total_length) {
        DBG("receiving total %d bytes\r\n", int(total_length));
        assert(total_length < 10000);
        missing_bytes = total_length;
    }
    void data(u8 *data, size_t length) {
        missing_bytes -= length;
        DBG("length %d missing %d\r\n", int(length), int(missing_bytes));
    }
    void crc(bool ok) {
        DBG("receiving crc %d\r\n", int(ok));
        // assert(ok);
        /* error, probably to due error on the serial transmission => retry ? */
    }
};

void gather_data() {
    C->update();
}

void send_data() {
    wcallback cb;
    TRACE();
    {
        usize L = 0;
        const auto *data = C->ticksReader()->adc_data(&L);
        if (data) {
            DBG("data[0]=%#08x\n", data[0]);
            auto p = W->post("http://192.168.178.22:8000/post/adc", data, L, &cb);
            if (p != 0)
                DBG("post result:%d\r\n", int(p)); // TODO error handling.
        } else
            return;
    }
    TRACE();
    debug::turnBuildinLED(true);
    if (C->total() > 10) {
        usize L = 0;
        TRACE();
        const u8 *data = C->data(&L);
        TRACE();
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/compteur", data, L, &cb);
            if (p != 0)
                DBG("post result:%d\r\n", int(p)); // TODO error handling.
            common::time::delay(10);
        }
    }
    TRACE();
    {
        usize L = 0;
        const auto *data = C->ticksReader()->histogram_data(&L);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/histogram", data, L, &cb);
            if (p != 0)
                DBG("post result:%d\r\n", int(p)); // TODO error handling.
        }
    }
    TRACE();
    {
        usize L = 0;
        const auto *data = status::instance.data(&L);
        DBG("data:%d %d", data[0], data[1]);
        if (data) {
            auto p = W->post("http://192.168.178.22:8000/post/status", data, L, &cb);
            if (p != 0)
                DBG("post result:%d\r\n", int(p)); // TODO error handling.
        }
    }
    TRACE();
    debug::turnBuildinLED(false);
}

void application::loop() {
    u32 t0 = common::time::since_reset();
    gather_data();
    send_data();
    u32 t1 = common::time::since_reset();
    if ((t1 - t0) < 100)
        common::time::delay(100 - (t1 - t0)); // adjusted to effectively get 100ms sampling rate
}
