#include "wifi.h"
#include "parse.h"
#include "nstring.h"

#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define espser Altser
#else
#define espser Serial
#endif

void sendData(const char * command) {
  espser.write(command);
  espser.write("\r\n");
}


parse::AccessPointParser app;
#define L 15
bool waitForResponse() {
  parse::StringAwaiter a("CLOSED");
  long int now = millis();
  long unsigned int deadline = now + 5000;
  char buffer[L+1]={0};
  Serial.println("RX...");
  while(millis()<deadline) {
    while(Altser.available()) {
      int n=Altser.readBytes(buffer,min(Altser.available(),L));
      buffer[n]='\0';
      if (a.read(buffer))
	return true; 
    }
  }
  return false;
}

boolean sendCommand(const char * command)
{
  sendData(command);
  delay(250);
  if (strstr(command,"UART_CUR")!=NULL)
    return true;
  long int now = millis();
  long unsigned int deadline = now + 5000;
  bool received = false;
  bool ok=false;
  bool error=false;

  char buffer[L+1]={0};
  
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");

  while(millis()<deadline && !received) {
    while(espser.available() && !received) {
      int n=espser.readBytes(buffer,min(espser.available(),L));
      buffer[n]='\0';

      if (strcmp(command,"AT+CWLAP")==0) {
	app.read(buffer);
      }
      
      ok=ok_wait.read(buffer);
      error=error_wait.read(buffer);
      received = ok || error;
    }
    
    if (strlen(buffer)) {
      Serial.print(buffer);
    }
    buffer[0]='\0';
  }
  
  if (received) {
  } else {
    Serial.println("timed out");
  }
  Serial.print("n=");
  Serial.println(app.size());
  Serial.println("-------------");
  delay(250);
  
  return ok;  
}


wifi::esp8266::esp8266()
  :timeout(3000) {
espser.begin(9600);
}

void wifi::esp8266::setTimeout(int t) {
  timeout=t;
}

bool wifi::esp8266::reset() {
  return false;
}


int wifi::test() {
  return 0;
}
