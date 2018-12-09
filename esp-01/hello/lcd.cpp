#include "Arduino.h"
#include "lcd.h"

#include <LiquidCrystal.h>
LiquidCrystal d(7, 5, 6, 10, 11, 12);

display::LCD::LCD() {
}

void display::LCD::init() {
  for(int d=0; d<3; ++d)
    {
      char msg[16]={0};
      snprintf(msg, 16, "INIT LCD: %d", d);    
      print(msg);
      delay(500);
    }
}

void display::LCD::print(char * msg) {
  d.clear();
  d.print(msg);
}

void display::LCD::print(const char * msg) {
  d.clear();
  d.print(msg);
}

//display::LCD display::lcd;
