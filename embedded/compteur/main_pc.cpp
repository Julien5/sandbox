#include "application.h"
#include "transmitter.h"

#include "common/tickscounter.h"
#include "common/time.h"
#include "common/stringawaiter.h"
#include "common/debug.h"
#include "compteur.h"
#include "histogram.h"

#include <thread>

int tests() {
    DBG("sizeof compteur:%d\n", sizeof(compteur));
    return histogram::Histogram::test();
    return tickscounter::test();
    return common::StringAwaiter::test();
    return compteur::test();
    return 0;
}

int main(int argc, char **argv) {
    //return tests();

    application::setup();
    std::thread serial_thread(transmitter::run);
    while (1) {
        application::loop();
        common::time::delay(1);
    }
    serial_thread.join();
    return 0;
}
