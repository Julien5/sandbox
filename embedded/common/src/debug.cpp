#include "common/debug.h"
#include "common/rusttypes.h"
#ifdef PC
#include <cstdint>
void debug::init_serial() {
}
void debug::turnBuildinLED(bool on) {
}
int debug::freeMemory() {
    return 0;
}
int thread_index() {
    const std::thread::id id = std::this_thread::get_id();
    static std::size_t nextindex = 0;
    static std::mutex my_mutex;
    static std::map<std::thread::id, std::size_t> ids;
    std::lock_guard<std::mutex> lock(my_mutex);
    if (ids.find(id) == ids.end())
        ids[id] = nextindex++;
    return ids[id];
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
    static bool init_done = false;
    if (!init_done) {
        gpio_config_t conf;
        conf.pin_bit_mask = GPIO_Pin_2;
        conf.intr_type = GPIO_INTR_DISABLE;
        conf.mode = GPIO_MODE_OUTPUT;
        conf.pull_up_en = GPIO_PULLUP_DISABLE;
        conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_config(&conf);
        init_done = true;
    }
    if (on)
        gpio_set_level(GPIO_NUM_2, 1);
    else
        gpio_set_level(GPIO_NUM_2, 0);
}
extern "C" {
#include "esp_system.h"
}
int debug::freeMemory() {
    return esp_get_free_heap_size();
}
#endif

#ifdef ARDUINO
#include "SoftwareSerial.h"

SoftwareSerial SOFT_UART(2, 3);
#define TXCHANNEL SOFT_UART
void serialprint(const char *buffer) {
    TXCHANNEL.print(buffer);
}
void serialprint(int x) {
    TXCHANNEL.print(x);
}
void serialflush() {
    TXCHANNEL.flush();
}
void debug::init_serial() {
    TXCHANNEL.begin(9600);
}
#define LED_DEBUG LED_BUILTIN
//#define LED_DEBUG 9
void debug::turnBuildinLED(bool on) {
    static bool init_done = false;
    if (!init_done) {
        pinMode(LED_DEBUG, OUTPUT);
        init_done = true;
    }
    if (on)
        digitalWrite(LED_DEBUG, HIGH);
    else
        digitalWrite(LED_DEBUG, LOW);
}

/* https://playground.arduino.cc/Code/AvailableMemory/ */
#include <Arduino.h>
extern unsigned int __heap_start;
extern void *__brkval;
struct __freelist {
    size_t sz;
    struct __freelist *nx;
};
extern struct __freelist *__flp;
int freeListSize() {
    struct __freelist *current;
    int total = 0;
    for (current = __flp; current; current = current->nx) {
        total += 2; /* Add two bytes for the memory block's header  */
        total += (int)current->sz;
    }
    return total;
}
int arduinoFreeMemory() {
    int free_memory;
    if ((int)__brkval == 0) {
        free_memory = ((int)&free_memory) - ((int)&__heap_start);
    } else {
        free_memory = ((int)&free_memory) - ((int)__brkval);
        free_memory += freeListSize();
    }
    return free_memory;
}
/* */
int debug::freeMemory() {
    return arduinoFreeMemory();
}

#endif

void debug::address_range(const char *text, void *o, size_t L) {
    u8 *i = (u8 *)o;
    DBG("%8s %ld:%ld [%ld]\r\n", text, i64(i), i64(i + L), i64(L));
}
