#include "AltSoftSerial.h"

AltSoftSerial Altser;

void sendData(const String &command) {
  Serial.println("send:"+command);
  const String s = command+"\r\n"; 
  Altser.write(s.c_str());
}

boolean sendCommand(String command)
{
  sendData(command);
  long int now = millis();
  long unsigned int deadline = now + 5000;
  String response;
  bool received = false;
  bool ok=false;
  bool error=false;
  while(millis()<deadline && !received) {
    while(Altser.available()) {
      char r=Altser.read();
      response+=r;
      if (r=='\n') {
	ok=response == "OK\r\n";
	error=response == "ERROR\r\n";
	received = ok || error;
	if (!received) {
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
    Serial.println("timed out (or error)");
  }
  
  return ok;  
}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
  while (!sendCommand("AT"));
  // sendCommand("AT+CWMODE_CUR=0");
}

int status=-1;

void loop() {
  bool ok=false;
  if (status == -1) {
     ok=sendCommand("AT+CWLAP");
     if (ok)
       status++;
  }
  if (status == 0) {
    delay(250);
    ok=sendCommand("AT+CWJAP_CUR=\"JBO\",\"00000000001111111111123456\"");
    delay(1000);
    if (ok) {
      ok=sendCommand("AT+CWJAP_CUR?");
      if (ok)
	status++;
    }
  }
  if (status == 1) {
    ok=sendCommand("AT+PING=\"192.168.2.1\"");
    if (ok) {
      ok=sendCommand("AT+CIPMUX?");
      ok=sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.1\",80");
      delay(1000);
      if (ok)
	status++;
    }
  }
  if (status == 2) {
    String cmd = "GET / HTTP/1.1";
    sendData("AT+CIPSEND=" + String(cmd.length() + 4)); // +2?
    sendCommand(cmd);
  }
  delay(1000);
}