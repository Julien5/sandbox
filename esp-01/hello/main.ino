#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
#endif
#include "LowPower.h"

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

bool wake_on_rising_reed=false;
const int reed_pin = 2;
long last_time_rising_reed=0;
void on_rising_reed() {
	wake_on_rising_reed=true;
}

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

  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
  
  stats.start(millis());
}

void upload_statistics() {
  unsigned long m = millis();
  statistics::data d;
  int length = 0;
  stats.getdata(m,d,&length);
  
  int trials = 3;
  while(trials-- >= 0 && length>=0) {
    if (esp.post("postrequest",d,length)) {
      display::lcd.print("result uploaded");
      const bool hard=true;
      stats.start(m,hard);
      break;
    }
  }
}

void print_count() {
  char msg[16];
  snprintf(msg, 128,"count: %d",stats.get_total_count());
  display::lcd.print(msg);
}

void sleep_now() {
  display::lcd.print("sleep");
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}

void loop() {
  long current_time=millis();
  int time_since_last_rising_reed = current_time-last_time_rising_reed;
  
  if (!wake_on_rising_reed && time_since_last_rising_reed>5000) {
    if (stats.elapsed(current_time) > 10000) // (1000*60*10) 10 minutes
      upload_statistics();
    sleep_now();
  }
   
  if (wake_on_rising_reed) {
    if (time_since_last_rising_reed>200)
      stats.increment_count(millis());
    last_time_rising_reed=current_time;
    wake_on_rising_reed=false;
    print_count();
  }  
}

