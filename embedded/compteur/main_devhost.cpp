#include "application.h"
#include "transmitter.h"

#include "common/tickscounter.h"
#include "common/time.h"
#include "ticksupdater.h"
#include "histogram.h"

#include <thread>

int tests() {
  //tickscounter::test();
  //histogram::Histogram::test();
  return TicksUpdater::test();
  return 0;
}

int main(int argc, char ** argv) {
  return tests();
  application::setup();
  //std::thread serial_thread(transmitter::run);
  while(1) {
    application::loop();
    Time::delay(1);
  }
  //serial_thread.join();
  return 0;
}

