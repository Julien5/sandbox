#include "sleep.h"

#if !defined(ESP8266)
#error implement me
#else
#include "espressif/esp_common.h"
#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_system.h>
#define SLEEP_1HR 0xD693A400
#define SLEEP_10SECS 10*1000*1000
void sleep::deep_sleep(const uint32_t &ms) {
  /* Clean all network connections */
  sdk_wifi_station_disconnect();

  /* Now just wait for the RTC to kill the CPU core */
  sdk_system_deep_sleep(ms*1000);
}
#endif
