#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
#endif

#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "statistics.h"

wifi::esp8266 esp;

void stop() {
  display::lcd.print("stop.");
  while(1);
}

statistics stats;

void reset() {
  stats.save();
  display::lcd.print("reset.");
  delay(1000);
  asm volatile ("jmp 0");
}

void setup()
{  
  display::lcd.init();
  display::lcd.print("init serial");
  Serial.begin(9600);
  while(!Serial);
  
  display::lcd.print("init ESP");
  if (!esp.reset())
    reset();
  if(!esp.join())
    reset();
  display::lcd.print("good.");
  delay(1000);
  stats.start(millis());
}

void upload_statistics() {
  statistics::data d;
  int length = 0;
  stats.getdata(d,&length);
  
  int trials = 3;
  while(trials-- >= 0) {
    if (esp.post("postrequest",d,length))
      display::lcd.print("result uploaded");
  }
}

int x=0;
void loop() {
  stats.increment_count(millis());
  upload_statistics();  
}

