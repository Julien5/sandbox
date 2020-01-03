#include "sleep.h"

#if defined(DEVHOST)
void sleep::deep_sleep(const uint32_t &ms) {
  DBG("deep sleep (=> delay)\n");
  delay(ms);
}

#elif defined(ARDUINO)
#error implement me for arduino

#elif defined(ESP8266)
#include <esp_system.h>
#include <esp_sleep.h>
void sleep::deep_sleep(const uint32_t &ms) {
  /* Clean all network connections */
  //sdk_wifi_station_disconnect();

  /* Now just wait for the RTC to kill the CPU core */
  esp_deep_sleep(ms*1000);
}
#endif
