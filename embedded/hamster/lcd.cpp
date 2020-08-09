#include "lcd.h"
#include "freememory.h"
#include "debug.h"
#include "platform.h"

#ifdef ARDUINO
#include "Arduino.h"
#include <LiquidCrystal.h>
#elif defined(DEVHOST)
#include <iostream>
#include <iomanip>
class LiquidCrystal {
  public:
    typedef uint8_t pin;
    LiquidCrystal(pin p1, pin p2, pin p3, pin p4, pin p5, pin p6){};
    void begin(int n, int m){};
    void setCursor(int n, int m){};
    void print(const std::string &s) {
        std::cout << "LCD: ";
        std::cout << std::setw(10);
        std::cout << millis() << " [" << s << "]\n";
    };
};
#else
class LiquidCrystal {
  public:
    typedef uint8_t pin;
    LiquidCrystal(pin p1, pin p2, pin p3, pin p4, pin p5, pin p6){};
    void begin(int n, int m){};
    void setCursor(int n, int m){};
    void print(const std::string &s){};
};
#endif

LiquidCrystal disp(7, 5, 6, 10, 11, 12);

display::LCD::LCD() {
}

void display::LCD::init() {
    disp.begin(16, 2);
    for (int d = 0; d < 3; ++d) {
        char msg[17] = {0};
        snprintf(msg, 17, "INIT LCD: %d", d);
        print(msg);
        delay(50);
    }
}

int min_free_memory = 2048;
int last_memory = 4096;
void display::LCD::print(const char *msg1, const char *msg2) {
    if (msg1) {
        char m1[17] = {0};
        snprintf(m1, 17, "%-16.16s", msg1);
        disp.setCursor(0, 0);
        disp.print(m1);
    }
    if (msg2) {
        char m2[17] = {0};
        snprintf(m2, 17, "%-16.16s", msg2);
        disp.setCursor(0, 1);
        disp.print(m2);
    }
}

void display::LCD::print(const char *msg1) {
    return print(msg1, 0);
}
