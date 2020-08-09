#include "application.h"
#include "common/debug.h"
#include "common/time.h"

void application::setup() {
    debug::init_serial();
    char *c = malloc(1);
    DBG("%d\r\n", int(c));
}

void application::loop() {
    DBG("hi\r\n");
    debug::turnBuildinLED(true);
    Time::delay(1000);
    debug::turnBuildinLED(false);
    Time::delay(1000);
}
