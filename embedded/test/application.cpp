#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"

void application::setup() {
    debug::init_serial();
#ifdef ARDUINO
    char *c = malloc(1);
    DBG("%d\r\n", int(c));
#endif
}

void application::loop() {
    DBG("hi\r\n");
    debug::turnBuildinLED(true);
    common::time::delay(1000);
    debug::turnBuildinLED(false);

    common::time::delay(100);
}
