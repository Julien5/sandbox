#include "common/debug.h"

#ifdef DEVHOST
void debug::turnBuildinLED(bool on) {
}
int debug::freeMemory() {
  return 0;
}
#endif

#ifdef ESP8266
#include "driver/gpio.h"
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
void debug::turnBuildinLED(bool on) {
  static bool init_done=false;
  if (!init_done) {
    pinMode(LED_BUILTIN, OUTPUT);
    init_done==true;
  }
  if (on)
    digitalWrite(LED_BUILTIN,HIGH);
  else
    digitalWrite(LED_BUILTIN,LOW);
}
extern char __heap_start;

int debug::freeMemory() {
  int free_memory;
  return ((int)&free_memory) - ((int)&__heap_start);
}
#endif