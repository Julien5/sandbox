#include "AltSoftSerial.h"

AltSoftSerial Altser;

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
}

int x=0;

void loop() {
  while(Altser.available())
    Serial.write(Altser.read());
  delay(250);
  Altser.write("AT\r\n");
}