#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"
#include "common/utils.h"
#include <string.h>

std::unique_ptr<common::serial> S;

void application::setup() {
    debug::init_serial();
    TRACE();
    S = std::unique_ptr<common::serial>(new common::serial);
    TRACE();
#ifdef ARDUINO
    analogReference(INTERNAL);
    //analogReference(DEFAULT);
#endif
}

#if defined(PC)
const char prefix[] = "<pc>";
#elif defined(ESP8266)
const char prefix[] = "<esp8266>";
#elif defined(ARDUINO)
const char prefix[] = "<arduino>";
#endif

void read() {
    DBG("reading ... ");
    u8 recv[256] = {0};
    while (true) {
        usize Lr = S->read(recv, sizeof(recv), 100);
        // we cannot rely on Lr.
        if (Lr > 0) {
            DBG("read Lr=%d bytes\r\n", int(Lr));
            utils::dump(recv, Lr);
            break;
        } else {
            DBG("nothing to read Lr=%d\r\n", int(Lr));
        }
    }
}

void write() {
    u8 send[32] = {0};
    //    snprintf((char *)send, sizeof(send), "%s[%d]%s", prefix, (int)Lr, (char *)recv);
    snprintf((char *)send, sizeof(send), "%s%s", prefix, prefix);
    DBG("writing '%s'... ", send);
    const auto Lw = S->write(send, strlen((char *)send));
    DBG("Lw=%d\r\n", int(Lw));
    // the write frequency should be (much) lower than the read frequency.
    //common::time::delay(common::time::ms(1000));
}

void application::loop() {
    DBG("loop(%d)\r\n", int(common::time::since_reset().value()));
    debug::turnBuildinLED(true);
    common::time::delay(common::time::ms(100));
#if 0
#if defined(ARDUINO)
    read(); //write();
#else
    write(); //read();
    common::time::delay(common::time::ms(100));
#endif
#ifdef ARDUINO
    auto ret = analogRead(0);
    DBG("common::analog::read:%d\r\n", int(ret));
#endif
#endif
    debug::turnBuildinLED(false);
    common::time::delay(common::time::ms(1000));
}
