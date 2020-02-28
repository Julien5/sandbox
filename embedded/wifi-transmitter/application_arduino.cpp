#include "application.h"
#include "debug.h"
#include "time.h"
#include "serial.h"

serial S;

void application::setup() {
  while(!Serial){};
  Serial.begin(9600);
  DBG("ok\r\n");
  S.init();
}

char data[8]={0};
char recb[16]={0};
int i=0;
uint16_t timeout=100;
void application::loop() {
  // send something
  sprintf(data,"test-%d",i++);
  DBG("tx: %s\r\n",data);
  S.write((uint8_t*) data,sizeof(data));
  // read response.
  memset(recb,0,sizeof(recb));
  S.read((uint8_t*)recb,sizeof(recb),timeout);
  DBG("rx: %s\r\n",recb);
  debug::turnBuildinLED(true);
  time::delay(200);
  debug::turnBuildinLED(false);
  time::delay(300);
}


