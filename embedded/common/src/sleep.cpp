#include "common/sleep.h"
#include "common/debug.h"
#include "common/time.h"

#if defined(DEVHOST)
void sleep::deep_sleep(const u32 &ms) {
    DBG("deep sleep (=> delay)\n");
    Time::delay(ms);
}
#endif

#if defined(ARDUINO)
#include "LowPower.h"
void sleep::deep_sleep(const u32 &ms) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
#endif

#if defined(ESP8266)
#include <esp_system.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include "time.h"
void sleep::deep_sleep(const u32 &ms) {
    /* Clean all network connections */
    esp_wifi_disconnect();
    esp_deep_sleep_set_rf_option(0);
    /* Now just wait for the RTC to kill the CPU core */
    esp_deep_sleep(ms * 1000);
    Time::delay(1000);
}
#endif
