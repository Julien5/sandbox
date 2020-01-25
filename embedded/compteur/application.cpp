#include "application.h"
#include "debug.h"
#include "time.h"
#include "sdcard.h"

sdcard sd;

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("@START");
#endif
  DBG("mem:%d\r\n",debug::freeMemory());
  sd.init();
  //sd.info();

}

bool done=false;
void application::loop()
{
  if (done)
    return;
      done=true;
  const char * d = "salut-tout-le-monde";
  sd.write("TEST2.TXT",(uint8_t*)d,strlen(d));
  DBG("mem:%d\r\n",debug::freeMemory());
}
