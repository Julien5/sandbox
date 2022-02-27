#include "common/sleep.h"
#include "common/debug.h"
#include "common/time.h"

#if defined(PC)
void sleep::deep_sleep(const common::time::ms &d) {
    common::time::delay(d);
}
#endif

#if defined(ARDUINO)
#include "LowPower.h"
void sleep::deep_sleep(const common::time::ms &delay) {
    //DBG("sleeping (down)\r\n");
    LowPower.longPowerDown(delay.value());
    //DBG("sleeping (up)\r\n");
    // sleeping..
    // ..
    // now wake up
    common::time::add_time_slept(delay);
}
#endif

#if defined(ESP8266)
#include <esp_system.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include "time.h"
void sleep::deep_sleep(const common::time::ms &delay) {
    /* Clean all network connections */
    esp_wifi_disconnect();
    esp_deep_sleep_set_rf_option(0);
    /* Now just wait for the RTC to kill the CPU core */
    esp_deep_sleep(delay.value() * 1000);
    common::time::delay(common::time::ms(1000));
}
#endif
