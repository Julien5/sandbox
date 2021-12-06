#include "application.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/platform.h"
#include "common/sleep.h"

#include "application.h"
#include "compteur.h"
#include "status.h"
#include "intermittentread.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<compteur> C;

void application::setup() {
    debug::init_serial();
    DBG(".");
#ifdef ARDUINO
    pinMode(espEnablePin, OUTPUT);
#endif
    C = std::unique_ptr<compteur>(new compteur);
}

common::time::us start_on;
common::time::us stop_on;

void application::loop() {
    C->update();
}
