#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"
#include <string.h>

std::unique_ptr<common::serial> S;

void application::setup() {
    debug::init_serial();
    TRACE();
    S = std::unique_ptr<common::serial>(new common::serial);
    TRACE();
#ifdef ARDUINO
    char *c = malloc(1);
    DBG("%d\r\n", int(c));
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
        if (strlen((char *)recv)) {
            DBG("'%s' Lr=%d\r\n", (char *)recv, int(Lr));
            break;
        } else {
            DBG("nothing to read\r\n");
        }
    }
}

void write() {
    u8 send[512] = {0};
    //    snprintf((char *)send, sizeof(send), "%s[%d]%s", prefix, (int)Lr, (char *)recv);
    snprintf((char *)send, sizeof(send), "%s%s", prefix, prefix);
    DBG("writing '%s'... ", send);
    const auto Lw = S->write(send, sizeof(send));
    DBG("Lw=%d\r\n", int(Lw));
    // the write frequency should be (much) lower than the read frequency.
    //common::time::delay(common::time::ms(1000));
}

void application::loop() {
    debug::turnBuildinLED(true);
    /*#if defined(ARDUINO)
    write();
#else
    read();
#endif
	*/
    DBG("hello\r\n");
    debug::turnBuildinLED(false);
    write();
    //common::time::delay(common::time::ms(1000));
    //debug::turnBuildinLED(false);
    //common::time::delay(common::time::ms(100));
}
