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
    TRACE();
    u8 recv[256] = {0};
    debug::turnBuildinLED(true);
    const usize Lr = S->read(recv, sizeof(recv), 1000);
    u8 send[512] = {0};
    snprintf((char *)send, xMin(Lr, sizeof(send)), "%s[%d]%s", prefix, (int)Lr, (char *)recv);
    debug::turnBuildinLED(false);
    common::time::delay(100);
}
