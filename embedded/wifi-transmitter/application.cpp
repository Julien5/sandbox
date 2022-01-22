#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/serial.h"

#include <string.h>

#include "message.h"

std::unique_ptr<common::serial> S = nullptr;

#ifdef PC
#include "wifi_curl.h"
#define WIFI wifi_curl
#else
#include "common/wifi.h"
#define WIFI wifi
#endif

namespace {
    std::unique_ptr<wifi::WIFI> W = nullptr;
}

void transmitter::setup() {
}

#include <chrono>

class serial_callback : public wifi::callback {
    common::serial *output;

  public:
    serial_callback(common::serial *_s)
        : output(_s){};
    void status(u8 s) {
        DBG("(1) forwarding status %d \n", int(s));
        auto ret = output->write(&s, sizeof(s));
        assert(ret == sizeof(s));
        DBG("(2) forwarding status ret:%d \n", int(ret));
        //common::time::delay(250);
    }
    void data_length(u16 total_length) {
        DBG("forwarding total %d bytes\n", total_length);
        output->write(reinterpret_cast<u8 *>(&total_length), sizeof(total_length));
    }
    void data(u8 *data, size_t length) {
        DBG("forwarding %d bytes\n", int(length));
        output->write(data, length);
    }
};

int k = 0;
void transmitter::loop_serial() {
    if (!S)
        S = std::unique_ptr<common::serial>(new common::serial);
    if (!W)
        W = std::unique_ptr<wifi::WIFI>(new wifi::WIFI);

    DBG("waiting for begin");

    debug::turnBuildinLED(true);
    TRACE();

    while (!S->wait_for_begin(common::time::ms(1000))) {
        TRACE();
    }

    bool ok = false;
    received::message m;
    ok = S->read_until(reinterpret_cast<u8 *>(&m.length), sizeof(m.length), 10);
    DBG("read serial: %d\n", m.length);
    if (!ok) {
        DBG("nothing to read.\n");
        return;
    }
    ok = S->read_until(m.data, m.length, 100);
    if (!ok) {
        DBG("failed to read data\n");
        return;
    }
    ok = S->check_end();
    if (!ok) {
        DBG("crc check failed\n");
        return;
    }
    received::wifi_command cmd = received::read_wifi_command(m);
    /* process cmd */
    DBG("process %s\n", cmd.url);
    S->begin();
    serial_callback cb(S.get());
    if (cmd.command == 'G')
        W->get(cmd.url, &cb);
    if (cmd.command == 'P') {
        DBG("Ldata:%d\n", cmd.Ldata);
        W->post(cmd.url, cmd.data, cmd.Ldata, &cb);
    }
    TRACE();
    S->end();
    TRACE();
}
