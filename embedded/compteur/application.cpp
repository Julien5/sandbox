#include "application.h"
#include "debug.h"
#include "time.h"
#include "sdcard.h"

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("@START");
#endif
  sdcard sd;
  sd.init();
  sd.info();
  const char * d = "ffff.ggg";
  sd.write("foo.txt",(uint8_t*)d,strlen(d));
}

void application::loop()
{
  DBG("mem:%d\r\n",debug::freeMemory());
  time::delay(250);
}
