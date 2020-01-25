#include "application.h"
#include "debug.h"
#include "time.h"
#include "sdcard.h"
#include "stdint.h"

#ifdef ARDUINO
#include "Arduino.h"
uint16_t analogRead() {
  return analogRead(0);
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

int data[256] = {0};
int indx=0;

void application::loop()
{ 
  int a=analogRead();
  data[indx++]=a;
  DBG("data[%d]=%d\r\n",indx,a);

  if (indx>=sizeof(data)/sizeof(int)) {
    auto ms = time::since_reset();
    int sec = ms/1000;
    char filename[12];
    sprintf(filename,"%d.bin",sec);
    DBG("writing %s\r\n",filename);
    sd.write(filename,(uint8_t*)data,sizeof(data));
    indx=0;
    DBG("mem:%d\r\n",debug::freeMemory()); 
  }
  
  time::delay(150);
}
