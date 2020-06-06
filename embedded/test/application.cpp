#include "application.h"
#include "common/debug.h"
#include "common/time.h"


void application::setup() {
  debug::init_serial();
}

void application::loop() {
  DBG("hi\r\n");
  debug::turnBuildinLED(true);
  Time::delay(1000);
  debug::turnBuildinLED(false);
  Time::delay(1000);
}


