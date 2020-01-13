#include "application.h"
#include "debug.h"
#include "time.h"


void application::setup() {
}

void application::loop()
{
  for(int i=0; i<10; ++i) {
    time::delay(100-10*i);
    debug::turnBuildinLED(bool(i%2));
  }
}
