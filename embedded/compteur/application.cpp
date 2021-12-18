#include "application.h"
#include "common/analog.h"
#include "common/debug.h"
#include "common/serial.h"
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
    DBG("sizeof(compteur):%d\r\n", int(sizeof(compteur)));
    DBG("sizeof(wifi::wifi):%d\r\n", int(sizeof(wifi::wifi)));
    DBG("sizeof(common::serial):%d\r\n", int(sizeof(common::serial)));
    C = std::unique_ptr<compteur>(new compteur);
}

void application::loop() {
    if (C->update())
        C->print();
}
