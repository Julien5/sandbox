#include "message.h"
#include "application.h"
#include "transmitter.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <cassert>

void wait(int n = 500) {
    std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

void transmitter::run() {
    while (!transmitter::stopped()) {
        transmitter::loop_serial();
        wait(250);
    }
}

#ifndef NOMAIN
int main(int, char **) {
    //received::test();
    transmitter::setup();

    std::thread serial_thread(transmitter::run);
    serial_thread.join();

    return 0;
}
#endif
