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

#define L 15

bool waitForResponse() {
  parse::StringAwaiter a("CLOSED");
  long int now = millis();
  long unsigned int deadline = now + 5000;
  char buffer[L+1]={0};
  Serial.println("RX...");
  while(millis()<deadline) {
    while(espser.available()) {
      int n=espser.readBytes(buffer,min(Altser.available(),L));
      buffer[n]='\0';
      Serial.print(buffer);
      if (a.read(buffer))
	return true; 
    }
  }
  return false;
}

boolean sendCommand(const char * command, const int length, const int timeout)
{
  delay(250);

  Serial.write(command,length);
  Serial.write("\r\n");
  espser.write(command,length);
  espser.write("\r\n");
  
  display::lcd.print(command);
  if (strstr(command,"UART_CUR")!=NULL)
    return true;

  long unsigned int now = millis();
  long unsigned int deadline = now + timeout;

  bool received=false;
  bool ok=false;
  bool error=false;

  char buffer[L+1]={0};
  
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");

  while(millis()<deadline && !received) {
    while(espser.available() && !received) {
      int n=espser.readBytes(buffer,min(espser.available(),L));
      buffer[n]='\0';
      Serial.print(buffer);
      ok=ok_wait.read(buffer);
      error=error_wait.read(buffer);
      received = ok || error;
    }
    buffer[0]='\0';
  }
  Serial.print("#");
  return ok;  
}

boolean sendCommand(const char * command, const int timeout) {
  return sendCommand(command,strlen(command),timeout);
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
  
  char request[128]={0};
  snprintf(request, 128, "GET %s HTTP/1.1\r\n\r\n", req);

  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", strlen(request)+2);
    
  if (!sendCommand(cipsend,short_timeout))
    return false;
  
  if (!sendCommand(request,long_timeout))
    return false;

  if (!waitForResponse())
    return false;
  
  sendCommand("AT+CIPCLOSE",short_timeout);
  return true;
}


bool wifi::esp8266::post(const char * req, const char * data, const int Ldata) {
  sendCommand("AT+CIPCLOSE",short_timeout); // ignore the result.
  if (!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.62\",8000",long_timeout))
    return false;
  
  char contentlength[32]={0};
  snprintf(contentlength, 32, "Content-Length: %d", Ldata);

  char request[256]={0};
  snprintf(request, 128,
	   "POST %s HTTP/1.1\r\n"
	   "%s\n"
	   "%s\n"
	   "%s\n"
	   "%s\n"
	   "\n",
	   req,
	   "Host: 192.168.2.62",
	   "Accept: */*",
	   contentlength,
	   "Content-Type: application/octet-stream"
	   );
  int k = strlen(request);
  for(int c=0; c<Ldata; k++,c++)
    request[k]=data[c];
  request[k++]='\r';
  request[k++]='\n';

  const int Lr = k;
  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", Lr+2);
  if (!sendCommand(cipsend,short_timeout))
    return false;

  if (!sendCommand(request,Lr,long_timeout))
    return false;
  
  if (!waitForResponse())
    return false;

  sendCommand("AT+CIPCLOSE",short_timeout);
  return true;
}


int wifi::test() {
  return 0;
}
