#include "AltSoftSerial.h"

AltSoftSerial Altser;

boolean sendCommand(String command)
{
  Serial.println(("send:")+command);
  const String s = command+"\r\n"; 
  Altser.write(s.c_str());
  long int now = millis();
  long int deadline = now + 1000;
  String response;
  bool received = false;
  while(millis()<deadline && !received) {
    while(Altser.available()) {
      char r=Altser.read();
      response+=r;
      if (r=='\n') {
	if ((response == "OK\r\n") || (response == "ERROR\r\n"))
	  received=true;
	else {
	  response="";
	}
	break;
      }
    }
  }
  Serial.print("received:");
  Serial.println(response);
  return received;  
}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
  sendCommand("AT");
}

void loop() {
}