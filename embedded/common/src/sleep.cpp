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
// from https://github.com/LowPowerLab/LowPower/blob/master/LowPower.cpp
void longPowerDown(uint32_t sleepTime) {
  do {
    if (sleepTime > 8000)
    {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      sleepTime-=8000;
    }
    else if (sleepTime > 4000)
    {
      LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
      sleepTime-=4000;
    }
    else if (sleepTime > 2000)
    {
      LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
      sleepTime-=2000;
    }
    else if (sleepTime > 1000)
    {
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
      sleepTime-=1000;
    }
    else if (sleepTime > 512)
    {
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      sleepTime-=512;
    }
    else if (sleepTime > 256)
    {
      LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
      sleepTime-=256;
    }
    else if (sleepTime > 128)
    {
      LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
      sleepTime-=128;
    }
    else if (sleepTime > 64)
    {
      LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
      sleepTime-=64;
    }
    else if (sleepTime > 32)
    {
      LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
      sleepTime-=32;
    }
    else if (sleepTime > 16)
    {
      LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
      sleepTime-=16;
    }
    else
    {
      sleepTime=0;
    }
  } while(sleepTime);
}
void sleep::deep_sleep(const common::time::ms &delay) {
  //DBG("sleeping (down)\r\n");
  longPowerDown(delay.value());
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
