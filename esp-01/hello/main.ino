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

void setup()
{  
  display::lcd.init();
  display::lcd.print("init serial");
  Serial.begin(9600);
  while(!Serial);
  
  display::lcd.print("init ESP");
  if (!esp.reset())
    stop();
  display::lcd.print("join.");
  if(!esp.join())
    stop();
  delay(1000);
}

int x=0;
void loop() {
    char cmd[128]={0};
    snprintf(cmd, 128, "/set?x=%d", x++);
    esp.get(cmd);
    delay(5000);
}