#include "application.h"
#include "transmitter.h"

#include <thread>

int main(int argc, char ** argv) {
  application::setup();
  std::thread serial_thread(transmitter::run);
  while(1)
    application::loop();
  serial_thread.join();
  return 0;
}

