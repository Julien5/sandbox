#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
#endif

#include "parse.h"
#include "wifi.h"
#include "lcd.h"

wifi::esp8266 esp;

void stop() {
  display::lcd.print("stop.");
  while(1);
}

void reset() {
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
  display::lcd.print("join.");
  if(!esp.join())
    reset();
  display::lcd.print("good.");
  delay(1000);
}

int x=0;
void loop() {
  char cmd[128]={0};
  snprintf(cmd, 128, "x=%d", x++);
  esp.post("postrequest",cmd);
}