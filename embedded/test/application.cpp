#include "application.h"
#include "debug.h"
#include "time.h"
void application::setup() {
}

void application::loop() {
  debug::turnBuildinLED(true);
  time::delay(1000);
  debug::turnBuildinLED(false);
  time::delay(1000);
}


