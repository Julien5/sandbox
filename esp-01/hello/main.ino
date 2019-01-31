#include "LowPower.h"
#include "debug.h"
#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "tickscounter.h"
#include "freememory.h"
#include "clock.h"

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
  display::lcd.print("good");
  delay(1000);
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
}

char try_upload_statistics(wifi::esp8266 &esp) {
  display::lcd.print("uploading...");
  int length=0;
  uint8_t * data = counter.getdata(&length);
  int ret=esp.post("tickscounter",data,length);
  if (ret != 0) {
    char msg[16];
    snprintf(msg, 16,"post error: %d",ret);
    display::lcd.print(msg);
    delay(200);
    return 1;
  }
  display::lcd.print("result uploaded");
  return 0;
}


bool upload_statistics() {
  if (counter.empty()) {
    display::lcd.print("nothing to upload");
    return true;
  }
  wifi::esp8266 esp(wifi_enable_pin);
  if (!esp.enabled())
    return false;
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

char * message() {
  static char * m=0;
  if (!m) {
    wifi::esp8266 esp(wifi_enable_pin);
    if (!esp.enabled())
      return 0;
    if (!esp.get("message",m))
      return 0;
    DBGTX("m={");DBGTX(m);DBGTXLN("}");
  }
  return m;
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
    snprintf(line1,17,"%u for %02d:%02d  %2d",counter.total(),h,m,counter.bin_count());
 
  char line2[17]={0};
  if (message()) 
    snprintf(line2,17,"%s",message());    
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
    if (counter.empty() || counter.recently_active())
      return;

    if (time_last_upload_failed>0) {
      const bool wifi_down = current_time - time_last_upload_failed < 5*one_minute;
      if (wifi_down)
	return;
    }
    
    if (!upload_statistics()) {
      time_last_upload_failed=current_time;
      display::lcd.print("upload failed");
    } else {
      time_last_upload_failed=0;
      display::lcd.print("upload good");
      reset();
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
