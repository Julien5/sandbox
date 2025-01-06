#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"
#include "common/utils.h"
#include <string.h>

#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

std::unique_ptr<common::serial> S;

void application::setup() {
    debug::init_serial();
#ifdef ARDUINO
    analogReference(INTERNAL);
    // analogReference(DEFAULT);
#endif
}

#if defined(PC)
const char prefix[] = "<pc>";
#elif defined(ESP8266)
const char prefix[] = "<esp8266>";
#elif defined(ARDUINO)
const char prefix[] = "<arduino>";
#endif

void application::loop() {
    DBG("loop(%d)\r\n", int(common::time::since_reset().value()));
    debug::turnBuildinLED(true);
    common::time::delay(common::time::ms(100));
#if 0
#if defined(ARDUINO)
    read(); //write();
#else
    write(); //read();
    common::time::delay(common::time::ms(100));
#endif
#ifdef ARDUINO
    auto ret = analogRead(0);
    DBG("common::analog::read:%d\r\n", int(ret));
#endif
#endif
    debug::turnBuildinLED(false);
    common::time::delay(common::time::ms(250));
}
