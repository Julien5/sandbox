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
  //w.join();
  //w.post("/test/foo/bar",(uint8_t*)d,strlen(d));
#endif
}

int data[64] = {0};
int indx=0;

void application::loop()
{
  // TRACE();
  auto a=analogRead();
  DBG("a=%d\r\n",a);
  data[indx]=a;
  
  debug::turnBuildinLED(bool(indx%2));

  indx++;
  if (indx>=sizeof(data)/sizeof(int))
    indx=0;

  time::delay(200);
}
