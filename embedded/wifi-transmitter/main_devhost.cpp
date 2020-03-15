#include "message.h"
#include "application.h"

#include <chrono>
#include <thread>

void wait(int n=500) {
  std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

void arduino() {
  while (1) {
    application::loop();
    wait();
  }
}

void serial() {
  while (1) {
    application::loop_serial();
    wait(250);
  }
}

void wifi() {
  while (1) {
    application::loop_wifi();
    wait();
  }
}

int main(int argc, char ** argv) {
  received::test();
  
  application::setup();
  
  std::thread arduino_thread(arduino);
  std::thread serial_thread(serial);
  std::thread wifi_thread(wifi);
  arduino_thread.join();
  serial_thread.join();
  wifi_thread.join();
  
  return 0;
}
