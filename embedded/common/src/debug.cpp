#include "common/debug.h"

#ifdef DEVHOST
void debug::init_serial() {
}
void debug::turnBuildinLED(bool on) {
}
int debug::freeMemory() {
  return 0;
}
#endif

#ifdef ESP8266
#include "driver/gpio.h"
// for arduino. For ESP, i don't remember
// if it must be filled with anything.
void debug::init_serial() {
  // FIXME
  // assert(0);
}
void debug::turnBuildinLED(bool on) {
  static bool init_done=false;
  if (!init_done) {
    gpio_config_t conf;
    conf.pin_bit_mask = GPIO_Pin_2;
    conf.intr_type = GPIO_INTR_DISABLE;
    conf.mode = GPIO_MODE_OUTPUT;
    conf.pull_up_en = GPIO_PULLUP_DISABLE;
    conf.pull_down_en =GPIO_PULLDOWN_DISABLE;
    gpio_config(&conf);
    init_done=true;
  }
  if (on)
    gpio_set_level(GPIO_NUM_2,1);
  else
    gpio_set_level(GPIO_NUM_2,0);
}
extern "C" {
#include "esp_system.h"
}
int debug::freeMemory() {
  return esp_get_free_heap_size();
}
#endif

#ifdef ARDUINO
#include "Arduino.h"
void debug::init_serial() {
  Serial.begin(9600);
}
void debug::turnBuildinLED(bool on) {
  static bool init_done=false;
  if (!init_done) {
    pinMode(LED_BUILTIN, OUTPUT);
    init_done=true;
  }
  if (on)
    digitalWrite(LED_BUILTIN,HIGH);
  else
    digitalWrite(LED_BUILTIN,LOW);
}

extern char *__brkval;
 
int debug::freeMemory() {
  char top;
  return &top - __brkval;
}

#endif

void debug::address_range(const char * text, void *o, size_t L) {
  uint8_t* i = (uint8_t*)o;
  DBG("%8s %d:%d [%d]\r\n",text,int(i),int(i+L),int(L));
}
