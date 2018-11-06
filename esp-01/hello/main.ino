#include "AltSoftSerial.h"

AltSoftSerial Altser;

void sendData(const String &command) {
  Serial.println("send:"+command);
  const String s = command+"\r\n"; 
  Altser.write(s.c_str());
}

String waitForResponse() {
  long int now = millis();
  long unsigned int deadline = now + 2500;
  String response;
  while(millis()<deadline) {
    while(Serial.available()) {
      char r=Serial.read();
      response+=r;
      Serial.write(r);
    }
  }
  return response;
}

boolean sendCommand(String command, String okString, String errorString)
{
  sendData(command);
  long int now = millis();
  long unsigned int deadline = now + 5000;
  String response;
  bool received = false;
  bool ok=false;
  bool error=false;
  while(millis()<deadline && !received) {
    while(Serial.available()) {
      char r=Serial.read();
      response+=r;
      if (r=='\n') {
	ok=response == okString+"\r\n";
	error=response == errorString+"\r\n";
	received = ok || error;
	if (!received) {
	  Serial.println("got:"+response);
	  response="";
	}
	break;
      }
    }
  }
  if (received)
    Serial.println("received:"+response);
  
  if (ok) {
    Serial.println("ok");
  } else if (error) {
    Serial.println("error detected");
  } else {
    Serial.println("timed out");
  }
  Serial.println("-------------");
  delay(250);
  return ok;  
}

boolean sendCommand(String command)
{
  return sendCommand(command,"OK","ERROR");
}

void work() {
  bool ok=false;
  while(!sendCommand("AT+CWLAP"));
  while(!sendCommand("AT+CWJAP_CUR=\"JBO\",\"00000000001111111111123456\""))
    delay(1000);
  while(!sendCommand("AT+CWJAP_CUR?"));
  while(!sendCommand("AT+PING=\"192.168.2.62\""));
  while(!sendCommand("AT+CIPMUX?"));
  while(!sendCommand("AT+CIPMODE?"));
  
}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
  while (!sendCommand("AT"));
  while (!sendCommand("AT+RST"));
  work();
}

int x=0;
int status=0;
String s;
char buffer[128];
int n=0;

void loop() {
  if (status==0) {
    while(!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.62\",8000"));
    String cmd = "GET /set?x="+String(x++)+" HTTP/1.1\r\n\r\n";
    while(!sendCommand("AT+CIPSEND=" + String(cmd.length() + 2),"OK","ERROR"))
      delay(1000); // +2?
    while(!sendCommand(cmd,"SEND OK","ERROR"));
    status=1;
    return;
  }
  if (status==1) {
    waitForResponse();
    status=2;
  }
  if (status==2) {
    Serial.print("done");
    sendCommand("AT+CIPCLOSE");
    delay(5000);
    status=0;
  }
}