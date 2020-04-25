#include "application.h"
#include "transmitter.h"

#include "common/tickscounter.h"

#include <thread>

int main(int argc, char ** argv) {
  return tickscounter::test();
  application::setup();
  std::thread serial_thread(transmitter::run);
  while(1)
    application::loop();
  serial_thread.join();
  return 0;
}

