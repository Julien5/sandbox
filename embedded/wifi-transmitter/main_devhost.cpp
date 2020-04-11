#include "message.h"
#include "application.h"
#include "crc.h"

#include <chrono>
#include <thread>

void wait(int n=500) {
  std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

void arduino() {
  while (1) {
    application::loop_arduino();
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

int main(int, char **) {
  received::test();
  crc::test();
  return 0;
  application::setup();
  
  std::thread arduino_thread(arduino);
  std::thread serial_thread(serial);
  std::thread wifi_thread(wifi);
  arduino_thread.join();
  serial_thread.join();
  wifi_thread.join();
  
  return 0;
}
