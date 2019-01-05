#include "LowPower.h"

#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "statistics.h"
#include "freememory.h"
#include "clock.h"

void stop() {
  display::lcd.print("stop.");
  while(1);
}

statistics stats;

bool wake_on_rising_reed=false;

const int reed_pin = 2;
const int wifi_enable_pin = 3;

/* we seem to have 870 bytes working place.
char buffer[768]={0};
char full[75+3+24]={0};
*/

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

bool try_upload_statistics(wifi::esp8266 &esp) {
  display::lcd.print("uploading...");
  int length=0;
  uint8_t * data = stats.getdata(&length);
  int ret=esp.post("postrequest",data,length);
  if (ret != 0) {
    char msg[16];
    snprintf(msg, 16,"error: %d",ret);
    display::lcd.print(msg);
    delay(200);
    return false;
  }
  
  stats.reset();

  // any get request returns a time stamp
  bool ok=esp.get("X");
  if (!ok)
    return false;

  char h=0,m=0,s=0;
  ok=esp.get_time(&h,&m,&s);
  if (!ok)
    return false;
  
  Clock::set_time(h,m,s);
  return true;
}

void upload_statistics() {
  wifi::esp8266 esp(wifi_enable_pin);
  int trials = 3;
  while(trials-- > 0) {
    if (try_upload_statistics(esp))
      break;
  }
  display::lcd.print("result uploaded");
  delay(200);
}

void print_count() {
  char msg[16];
  snprintf(msg, 16,"count: %d",stats.total());
  display::lcd.print(msg);
}

Clock::ms sleep_duration = 0;
void sleep_now() {
  display::lcd.print("sleep");
  sleep_duration = 8000;
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}

Clock::ms last_time_rising_reed=0;

void loop() {
  Clock::ms current_time=Clock::since_start();
  Clock::ms time_since_last_rising_reed = current_time-last_time_rising_reed;

  if (!wake_on_rising_reed) {
    // wake after sleep;
    Clock::wake_up_after(sleep_duration);
  }
  
  if (!wake_on_rising_reed) {
    // 10 minutes without sensor activity => seems we can upload.
    if (time_since_last_rising_reed>10*60*1000L)
      upload_statistics();
    sleep_now();
  }
  
  if (wake_on_rising_reed) {
    if (time_since_last_rising_reed>200) // avoid interrupt bouncing
      stats.tick();
    
    last_time_rising_reed=current_time;
    
    wake_on_rising_reed=false;
    print_count();
  } 
}

