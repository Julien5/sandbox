#include "application.h"
#include "transmitter.h"

#include "common/tickscounter.h"
#include "ticksupdater.h"
#include "histogram.h"

#include <thread>

int main(int argc, char ** argv) {
  tickscounter::test();
  Histogram::test();
  TicksUpdater::test();
  return 0;
  application::setup();
  std::thread serial_thread(transmitter::run);
  while(1)
    application::loop();
  serial_thread.join();
  return 0;
}

