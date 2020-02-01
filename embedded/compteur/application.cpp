#include "application.h"
#include "debug.h"
#include "time.h"
#include "sdcard.h"
#include "stdint.h"

#include <string.h>
#ifdef ARDUINO
#include "Arduino.h"
uint16_t analogRead() {
  return analogRead(0);
}
#else
uint16_t analogRead() {
  return 1;
}

#endif

sdcard sd;

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("@START");
#endif
  
  sd.init();
  sd.info();
  const char * d = "ffff.ggg";
  sd.write("foo.txt",(uint8_t*)d,strlen(d));
}

uint16_t data[256] = {0};
uint16_t indx=0;
uint16_t counter=0;

void application::loop()
{ 
  int a=analogRead();
  data[indx++]=a;
  DBG("data[%d]=%d\r\n",indx,a);

  if (indx>=sizeof(data)/sizeof(int)) {
    char filename[13]; // 8.3 => 8+1+3+1 (zero termination) => 13 bytes.
    sprintf(filename,"%08u.BIN",counter++);
    DBG("writing %s\r\n",filename);
    sd.write(filename,(uint8_t*)data,sizeof(data));
    indx=0;
    DBG("mem:%d\r\n",debug::freeMemory()); 
  }
  time::delay(50);
}
