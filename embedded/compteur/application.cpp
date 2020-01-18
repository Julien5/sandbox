#include "application.h"
#include "debug.h"
#include "time.h"
#include "stdint.h"
#include "wifi.h"


#ifdef ARDUINO
#include "Arduino.h"
uint16_t analogRead() {
  return analogRead(0);
}
#endif

#ifdef ESP8266
#include "driver/adc.h"
uint16_t analogRead() {
  uint16_t ret=0;
  adc_read(&ret);
  return ret;
}
#endif

#include <string.h>

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  Serial.println("@START");
#endif

#ifdef ESP8266
  adc_config_t config;
  config.mode=ADC_READ_TOUT_MODE;
  config.clk_div=8;
  auto err=adc_init(&config);
  DBG("err=%d\r\n",err);

  const char * d = "salut tout le monde";
  wifi::wifi w;
  w.join();
  w.post("/test/foo/bar",(uint8_t*)d,strlen(d));
#endif
}


void application::loop()
{
  // TRACE();
  auto a=analogRead();
  DBG("a=%d\r\n",a);
  // time::delay(a);
  // debug::turnBuildinLED(bool(i%2));
  
  time::delay(50);
}
