#include "application.h"
#include "../wifi-transmitter/transmitter.h"

#include "common/tickscounter.h"
#include "common/time.h"
#include "common/stringawaiter.h"
#include "common/debug.h"
#include "compteur.h"

#include <thread>

#include "parameters.h"

int tests() {
    DBG("sizeof compteur:%d\n", int(sizeof(compteur)));
    if (Detection::test() != 0)
        return 1;
    if (tickscounter::test() != 0)
        return 2;
    if (common::StringAwaiter::test() != 0)
        return 3;
    if (compteur::test() != 0)
        return 4;
    return 0;
}

int main(int argc, char **argv) {
    auto code = tests();
    if (code != 0)
        return code;
    parameters::set(argc, argv);
    application::setup();
    std::thread serial_thread(transmitter::run);
    while (1) {
        application::loop();
        common::time::simulate(common::time::us(10));
    }
    serial_thread.join();
    return 0;
}
