#include "sleep.h"

#if defined(DEVHOST)
void sleep::deep_sleep(const uint32_t &ms) {
  DBG("deep sleep (=> delay)\n");
  delay(ms);
}
#endif

#if defined(ARDUINO)
#error implement me for arduino
#endif

#if defined(ESP8266)
#include <esp_system.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
void sleep::deep_sleep(const uint32_t &ms) {
  /* Clean all network connections */
  esp_wifi_disconnect();
  // esp_deep_sleep_set_rf_option(4);
  /* Now just wait for the RTC to kill the CPU core */
  esp_deep_sleep(ms*1000);
  while(1){};
}
#endif
