#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"

#include "application.h"
#include "compteur.h"

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
        assert(s == 0);
        DBG("memory:%d\r\n", debug::freeMemory());
    }
    void data_length(u16 total_length) {
        DBG("receiving total %d bytes\r\n", int(total_length));
        assert(total_length < 10000);
        missing_bytes = total_length;
    }
    void data(u8 *data, size_t length) {
        DBG("length %d missing %d\r\n", int(length), int(missing_bytes));
        missing_bytes -= length;
    }
    void crc(bool ok) {
        DBG("receiving crc %d\r\n", int(ok));
        assert(ok);
        /* error, probably to due error on the serial transmission => retry ? */
    }
};

void gather_data() {
    C->update();
}

void send_data() {
    //if (C->total() < 1)
    //     return;
    wcallback cb;
    //auto p=W->get("http://example.com/",&cb);
    //Time::delay(10);
    // u8 data[4]={0x01,0x02,0x03,0x04};
    size_t L = 0;
    const u8 *data = C->data(&L);
    auto p = W->post("http://192.168.178.22:8000/post/", data, L, &cb);
    if (p != 0)
        DBG("post result:%d\r\n", int(p)); // TODO error handling.
    Time::delay(10);
}

void application::loop() {
    debug::turnBuildinLED(false);
    gather_data();
    send_data();
    debug::turnBuildinLED(false);
    DBG("\r\n");
    Time::delay(200);
}
