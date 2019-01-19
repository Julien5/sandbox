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
  display::lcd.print("good");
  delay(1000);
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
}

char try_update_time(wifi::esp8266 &esp) {
  display::lcd.print("getting time...");
  bool ok=esp.get("time");
  if (!ok) {
    display::lcd.print("GET error");
    return 2;
  }
  return 0;
}

char try_upload_statistics(wifi::esp8266 &esp) {
  display::lcd.print("uploading...");
  int length=0;
  uint8_t * data = stats.getdata(&length);
  int ret=esp.post("postrequest",data,length);
  if (ret != 0) {
    char msg[16];
    snprintf(msg, 16,"post error: %d",ret);
    display::lcd.print(msg);
    delay(200);
    return 1;
  }
  display::lcd.print("result uploaded");
  stats.reset();
  ret=try_update_time(esp);
  if (ret!=0)
    return ret;
  return 0;
}

bool update_time() {
  wifi::esp8266 esp(wifi_enable_pin);
  if (!esp.enabled())
    return false;
  return try_update_time(esp) == 0;
}

bool upload_statistics() {
  if (stats.day_total() == 0) {
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


Clock::ms last_update_display=0;
void update_display() {
  Clock::ms t=Clock::millis_today();
  if ((t-last_update_display)<1000)
    return;
  last_update_display=t;
  
  types::minute m=stats.last_minute();
  char h=m/60;
  m-=60*h;
  
  char line1[16]={0};
  if (stats.day_total()>0)
    snprintf(line1,16,"%u at %02d:%02d",stats.day_total(),h,m);
 
  char line2[16]={0}; 
  snprintf(line2,16,"TOTAL=%u",stats.full_total());
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

void loop() {
  Clock::ms current_time=Clock::since_start();
  Clock::ms time_since_last_rising_reed = current_time-last_time_rising_reed;

  if (!wake_on_rising_reed && slept) {
    // wake after sleep;
    Clock::wake_up_after(sleep_duration);
    slept=false;
  }


  if (!Clock::good()) {
    while(!update_time())
      display::lcd.print("updating time...");
  }
  
  update_display();
 
  if (!wake_on_rising_reed) {
    constexpr Clock::ms no_activity_time_for_upload = 10*1000L; //10*60*1000L;
    // 10 minutes without sensor activity => seems we can upload.
    if (time_since_last_rising_reed>no_activity_time_for_upload) {
      if (stats.day_total()==0)
	return;
      if (!upload_statistics()) {
	display::lcd.print("upload failed");
      } else {
	display::lcd.print("upload good");
      }
    }
    sleep_now();
  }
  
  if (wake_on_rising_reed) {
    DBGTXLN("tick.");
    if (time_since_last_rising_reed>200) // avoid interrupt bouncing
      stats.tick();
    last_time_rising_reed=current_time;
    wake_on_rising_reed=false;
  } 
}

