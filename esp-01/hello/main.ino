#include "LowPower.h"
#include "debug.h"
#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "tickscounter.h"
#include "freememory.h"
#include "clock.h"
#include <limits.h>

void stop() {
  display::lcd.print("stop.");
  while(1);
}

tickscounter counter;

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
  display::lcd.print("setup...");
  delay(1000);

  pinMode(reed_pin, INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  for(int k=0;k<5;k++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  attachInterrupt(0,on_rising_reed,RISING);
}


namespace message {
  char * addr = 0;
  bool update() {
    wifi::esp8266 esp(wifi_enable_pin);
    digitalWrite(LED_BUILTIN, LOW);
    if (!esp.enabled()) {
      digitalWrite(LED_BUILTIN, HIGH);
      return false;
    }
    if (!esp.get("message",&addr))
      return false;
    DBGTX("addr={");DBGTX(addr);DBGTXLN("}");
    return true;
  }
}

char try_upload_statistics(wifi::esp8266 &esp) {
  int length=0;
  uint8_t * data = counter.getdata(&length);
  int ret=esp.post("tickscounter",data,length,&message::addr);
  if (ret != 0) {
    char msg[16];
    snprintf(msg, 16,"post error: %d",ret);
    display::lcd.print(msg);
    delay(200);
    return 1;
  }
  display::lcd.print("result uploaded");
  counter.reset();
  return 0;
}

bool upload_statistics() {
  if (counter.empty()) {
    return true;
  }
  display::lcd.print("uploading...");
  wifi::esp8266 esp(wifi_enable_pin);
  digitalWrite(LED_BUILTIN, LOW);
  if (!esp.enabled()) {
    digitalWrite(LED_BUILTIN, HIGH);
    display::lcd.print("esp broken");
    delay(200);
    return false;
  }
  int trials = 3;
  char ret=1;
  while(trials-- > 0) {
    ret=try_upload_statistics(esp);
    if (ret == 0)
      return true;
    esp.reset();
    esp.join();
  }
  return false;
}

Clock::ms last_update_display=0;
void update_display() {
  Clock::ms t=Clock::millis_since_start();
  if ((t-last_update_display)<1000)
    return;
  last_update_display=t;
  
  bin::time m0=counter.last_tick_time();
  bin::time m1=Clock::minutes_since_start();
  bin::time m=m1-m0;
  char h=m/60;
  m-=60*h;

  char line1[17]={0};
  if (counter.total()>0)
    snprintf(line1,17,"%u for %02d:%02d",counter.total(),h,m);
  
  if (!message::addr) {
    message::update();
  }
  
  char line2[17]={0};
  if (message::addr) 
    snprintf(line2,17,"%s",message::addr);    
  else
    snprintf(line2,17,"%s","(no message)");
  display::lcd.print(line1,line2);
}

Clock::ms sleep_duration = 0;
bool slept=false;
void sleep_now() {
  slept=true;
#if 0
  // disabled, because the watchdog timer is too inacurate:
  // so sleep_duration is actually unknow :-(
  display::lcd.print("sleep");
  sleep_duration = 8000;
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
#endif
}

bool time_approaches_overflow() {
  return millis()>ULONG_MAX/2;
}

Clock::ms last_time_rising_reed=0;
Clock::ms time_last_upload_failed=0;

constexpr Clock::ms one_minute=60*1000L;
constexpr Clock::ms one_hour=60*one_minute;

void loop() {
  Clock::ms current_time=Clock::millis_since_start();
  Clock::ms time_since_last_rising_reed = current_time-last_time_rising_reed;

  if (!wake_on_rising_reed && slept) {
    // wake after sleep;
    Clock::wake_up_after(sleep_duration);
    slept=false;
  }

  update_display();
 
  if (!wake_on_rising_reed) {

    if (time_approaches_overflow()) {
      if (upload_statistics()) {
	reset();
	return;
      }
    }
    
    if (counter.empty() || counter.recently_active())
      return;

    if (time_last_upload_failed>0) {
      const bool wifi_down = (current_time - time_last_upload_failed) < 60*one_minute;
      if (wifi_down)
	return;
    }
    
    if (!upload_statistics()) {
      time_last_upload_failed=current_time;
      display::lcd.print("upload failed");
    } else {
      time_last_upload_failed=0;
      display::lcd.print("upload good");
    }    
    sleep_now();
  }
  
  if (wake_on_rising_reed) {
    Serial.print("tick:");Serial.println(time_since_last_rising_reed);
    if (time_since_last_rising_reed>350) // avoid interrupt bouncing
      counter.tick();
    last_time_rising_reed=current_time;
    wake_on_rising_reed=false;
  } 
}
