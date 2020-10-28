#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"

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

void application::loop() {
    // TRACE();
    u8 recv[256] = {0};
    common::time::delay(50);
    debug::turnBuildinLED(true);
    DBG("reading ... ");
    const usize Lr = S->read(recv, sizeof(recv), 100);
    DBG("'%s' Lr=%d\r\n", (char *)recv, int(Lr));
    u8 send[512] = {0};
    snprintf((char *)send, sizeof(send), "%s[%d]%s", prefix, (int)Lr, (char *)recv);
    // DBG("writing '%s'... ", send);
    // const auto Lw = S->write(send, sizeof(send));
    // DBG("Lw=%d\r\n", int(Lw));
    debug::turnBuildinLED(false);
    common::time::delay(100);
}
