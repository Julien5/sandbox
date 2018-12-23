#include "LowPower.h"

#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "statistics.h"
#include "freememory.h"

void stop() {
  display::lcd.print("stop.");
  while(1);
}

bool wake_on_rising_reed=false;

const int reed_pin = 2;
const int wifi_enable_pin = 3;
long last_time_rising_reed=0;

char buffer[790]={0};

void on_rising_reed() {
	wake_on_rising_reed=true;
}

void reset() {
  display::lcd.print("reset.");
  delay(1000);
  asm volatile ("jmp 0");
}

void setup()
{
  Serial.begin(9600);
  Serial.println("@START");
  display::lcd.init();
  delay(50);  
  display::lcd.print("good.");
  delay(1000);
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
}

void upload_statistics() {
  wifi::esp8266 esp(wifi_enable_pin);
  unsigned long m = millis();
  char * data = buffer;
  int length = sizeof(buffer);
  int trials = 3;
  for(int k=0; k<length; ++k)
    buffer[k]=0;
  while(trials-- > 0 && length>0) {
    display::lcd.print("uploading...");
    int ret=esp.post("postrequest",data,length);
    esp.get("weihnachten");
    if (ret==0) {
      display::lcd.print("result uploaded");
      delay(200);
      return;
    } else {
      char msg[16];
      snprintf(msg, 16,"error: %d %d",ret,trials);
      display::lcd.print(msg);
      delay(200);
    }
  }
  
}

void print_count() {
  char msg[16];
  snprintf(msg, 16,"count: %d",1);
  display::lcd.print(msg);
}

void sleep_now() {
  display::lcd.print("sleep");
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}

void loop() {
  {
    DBGTXLN("go");
    printMemory(0);
    upload_statistics();
    DBGTXLN("wait");
    delay(100);
  }
  return;
  /*long current_time=millis();
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
  */ 
}

