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

namespace get {
  bool message(wifi::esp8266 &esp, char * buffer) {
    char *internal=0;
    if (!esp.get("message",&internal)) {
      // retry ?
      /*esp.disable();if (!esp.enable()) return false; if (!esp.get("message",&internal)) return false;*/
      return false;
    }
    strncpy(buffer,internal,16);
    return true;
  }
  bool seconds_until_next_wifi(wifi::esp8266 &esp, uint32_t *buffer) {
    char *internal=0;
    if (!esp.get("sunw",&internal)) {
      return false;
    }
    if (sscanf(internal,"%d",buffer)!=1)
      return false;
    return true;
  }
}

void update_display_wifi(wifi::esp8266 &esp) {
  char line2[17]={0};
  if (!get::message(esp,line2))
    snprintf(line2,17,"%s","(no message)");
  display::lcd.print(0,line2);
}

char try_upload_statistics(wifi::esp8266 &esp) {
  int length=0;
  uint8_t * data = counter.getdata(&length);
  int ret=esp.post("tickscounter",data,length,0);
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

bool upload_statistics(wifi::esp8266 &esp) {
  if (counter.empty())
      return true;
  display::lcd.print("uploading...");
  int trials = 3;
  char ret=1;
  while(trials-- > 0) {
    ret=try_upload_statistics(esp);
    if (ret == 0) {
      return true;
    }
    esp.reset();
    esp.join();
  }
  return false;
}

bool time_approaches_overflow() {
  return millis()>ULONG_MAX/2;
}

uint32_t millis_next_upload = 0;
bool wifi_work() {
  if (millis()<=millis_next_upload || counter.recently_active())
    return true;

  display::lcd.print(0,"wifi...");
  
  wifi::esp8266 esp(wifi_enable_pin);
  digitalWrite(LED_BUILTIN, LOW);
  if (!esp.enabled()) {
    digitalWrite(LED_BUILTIN, HIGH);
    display::lcd.print("esp broken");
    delay(200);
    return false;
  }
  
  upload_statistics(esp);

  if (time_approaches_overflow()) 
    reset();

  uint32_t secs_until_next_wifi=0;
  if (get::seconds_until_next_wifi(esp,&secs_until_next_wifi))
    millis_next_upload = millis() + 1000L*secs_until_next_wifi;
  else
    millis_next_upload = millis() + 1000L*3600L; // one-hour

  update_display_wifi(esp);

  return true;
}

Clock::ms last_update_display=0;
int bin_indx=0;
void update_display_local() {
  Clock::ms t=Clock::millis_since_start();
  if ((t-last_update_display)<1000)
    return;
  last_update_display=t;

  if(bin_indx>=NTICKS || counter.getbin(bin_indx).empty())
    bin_indx=0;
  bin b=counter.getbin(bin_indx);

  int T=counter.total();
  int E=0;
  while(T>=100) {
    T=T/10;
    E++;
  }

  char line1[17]={0};
  if (counter.total()>0) 
    snprintf(line1,17,"%2de%1d %2dm %3d I%2d",
	     T,E,
	     int(1+b.m_duration/(1000L*60)),
	     int(b.m_count),
	     int(bin_indx+1));
  else
    snprintf(line1,17,"no ticks");
  display::lcd.print(line1,0);
  
  bin_indx++;
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

  update_display_local();
 
  if (!wake_on_rising_reed) {
    wifi_work();
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
