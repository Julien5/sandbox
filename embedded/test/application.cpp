#include "application.h"
#include "common/debug.h"
#include "common/time.h"

#ifdef ARDUINO
#include "Arduino.h"
void init_serial() {
  Serial.begin(9600);
}
#else
void init_serial() {
}
#endif


void application::setup() {
  init_serial();
}

void application::loop() {
  DBG("hi\r\n");
  debug::turnBuildinLED(true);
  Time::delay(1000);
  debug::turnBuildinLED(false);
  Time::delay(1000);
}


