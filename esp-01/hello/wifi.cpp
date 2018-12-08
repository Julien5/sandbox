#include "wifi.h"
#include "parse.h"
#include "nstring.h"
#include "lcd.h"

#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define espser Altser
#else
#define espser Serial
#endif



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

void sendData(const char * command) {
  espser.write(command);
  espser.write("\r\n");
}

boolean sendCommand(const char * command, const int timeout)
{
  delay(250);
  sendData(command);
  display::lcd.print(command);
  if (strstr(command,"UART_CUR")!=NULL)
    return true;

  long unsigned int now = millis();
  long unsigned int deadline = now + timeout;

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
    buffer[0]='\0';
  }
  return ok;  
}

void resetSerial(const int baudrate=9600) {
  espser.begin(baudrate);
  while(espser.available())
    espser.read();
}

const int short_timeout = 1000;
const int long_timeout = 20000;

wifi::esp8266::esp8266()
  :timeout(3000) {
  resetSerial();
}

void wifi::esp8266::setTimeout(int t) {
  timeout=t;
}

bool wifi::esp8266::reset() {
  resetSerial();
  return sendCommand("AT+RST",short_timeout);
}

bool wifi::esp8266::join() {
  return sendCommand("AT+CWJAP_CUR=\"JBO\",\"00000000001111111111123456\"",long_timeout);
}

bool wifi::esp8266::ping() {
  return sendCommand("AT+PING=\"192.168.2.62\"",long_timeout);
}

bool wifi::esp8266::get(const char * req) {
  if (!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.62\",8000",long_timeout))
    return false;

  char get[128]={0};
  snprintf(get, 128, "GET %s HTTP/1.1\r\n\r\n", req);
  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", strlen(get)+2);
  if (!sendCommand(cipsend,short_timeout))
    return false;
  
  if (!sendCommand(get,long_timeout))
    return false;

  if (!waitForResponse())
    return false;
  
  sendCommand("AT+CIPCLOSE",short_timeout);
  return true;
}



int wifi::test() {
  return 0;
}
