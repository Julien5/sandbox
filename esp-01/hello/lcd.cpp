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
      delay(250);
    }
}

int min_free_memory=2048;

void display::LCD::print(char * msg1) {
  d.setCursor(0,0);
  d.clear();
  d.print(msg1);
  d.setCursor(0,1);
  {
    int m = freeMemory();
    if (min_free_memory>m)
      min_free_memory=m;
    char c[16];
    snprintf(c, 16,"%d (min:%d) bytes",m,min_free_memory);
    d.print(c);
  }
}

void display::LCD::print(const char * msg) {
  print(const_cast<char*>(msg));
}
