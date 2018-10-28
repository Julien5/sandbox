#include "AltSoftSerial.h"

AltSoftSerial Altser;

boolean sendCommand(String command)
{
  Serial.println("send:"+command);
  const String s = command+"\r\n"; 
  Altser.write(s.c_str());
  long int now = millis();
  long unsigned int deadline = now + 1000;
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
	  Serial.println("got:"+response);
	  response="";
	}
	break;
      }
    }
  }
  if (received) {
    Serial.print("received:");
    Serial.println(response);
  } else {
    Serial.println("timed out");
  }
  
  return received;  
}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
  sendCommand("AT");
  sendCommand("AT+CWMODE_CUR=0");
}

void loop() {
  sendCommand("AT+CWLAP");
  delay(250);
  sendCommand("AT+CWJAP_CUR=\"JBO\",\"00000000001111111111123456\"");
}