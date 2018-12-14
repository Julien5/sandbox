#include "Arduino.h"
#include "lcd.h"
#include "freememory.h"

#include <LiquidCrystal.h>
LiquidCrystal d(7, 5, 6, 10, 11, 12);

display::LCD::LCD() {
}

void display::LCD::init() {
  d.begin(16,2);
  for(int d=0; d<3; ++d)
    {
      char msg[16]={0};
      snprintf(msg, 16, "INIT LCD: %d", d);    
      print(msg);
      delay(500);
    }
}

void display::LCD::print(char * msg1, char * msg2) {
  d.setCursor(0,0);
  d.clear();
  d.print(msg1);
  d.setCursor(0,1);
  if (msg2) {
    d.print(msg2);
  } else {
    char c[16];
    snprintf(c, 16,"free ram: %d b",stack::get_free_memory());
    d.print(c);
  }
}

/*
void display::LCD::print(char * msg) {
  print(msg,0);
}

void display::LCD::print(const char * msg) {
  print(const_cast<char*>(msg));
}

void display::LCD::print(const char * msg1, const char *msg2) {
  print(const_cast<char*>(msg1),const_cast<char*>(msg2));
}
*/

//display::LCD display::lcd;
