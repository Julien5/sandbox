#include "Arduino.h"
#include "lcd.h"
#include "freememory.h"
#include "debug.h"
#include <LiquidCrystal.h>
LiquidCrystal d(7, 5, 6, 10, 11, 12);

display::LCD::LCD() {
}

void display::LCD::init() {
  d.begin(16,2);
  for(int d=0; d<3; ++d)
    {
      char msg[17]={0};
      snprintf(msg, 17, "INIT LCD: %d", d);    
      print(msg);
      delay(50);
    }
}

int min_free_memory=2048;
int last_memory=4096;
void display::LCD::print(const char * msg1, const char *msg2) {
  DBGTX(msg1); DBGTX("#"); DBGTXLN(msg2);
  if (msg1) {
    char m1[17]={0};
    snprintf(m1,17,"%-16.16s",msg1);
    d.setCursor(0,0);
    d.print(m1);
  }  
  if (msg2) {
    char m2[17]={0};
    snprintf(m2,17,"%-16.16s",msg2);
    d.setCursor(0,1);
    d.print(m2);
  }
}

void display::LCD::print(const char * msg1) {
  return print(msg1,0);
}
