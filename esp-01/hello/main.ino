#include "AltSoftSerial.h"

AltSoftSerial Altser;

#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 5, 6, 10, 11, 12);

#include "parse.h"

void sendData(const char * command) {
  Altser.write(command);
  Altser.write("\r\n");
}

#define L 127

bool waitForResponse() {
  parse::StringAwaiter a("CLOSED");
  long int now = millis();
  long unsigned int deadline = now + 5000;
  char buffer[L+1]={0};
  Serial.println("RX...");

  while(millis()<deadline) {
    while(Altser.available()) {
      Altser.readBytes(buffer,min(Altser.available(),L));
      if (a.read(buffer))
	return true; 
    }
  }
  return false;
}

boolean sendCommand(const char * command)
{
  lcd.print(command);
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
  int n=0;
  
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");
    
  while(millis()<deadline && !received) {

    long int t0 = millis();    
    while((Altser.available()||(millis()-t0)<1000) && !received && n<L) {
      n+=Altser.readBytes(buffer+n,min(Altser.available(),L-n));
    }
    
    buffer[n]='\0';
    ok=ok_wait.read(buffer);
    error=error_wait.read(buffer);
    received = ok || error;
    
    if (n>0) {
      Serial.print("n=");
      Serial.print(n);
      Serial.print(" got:[");
      Serial.print(buffer);
      Serial.println("]");
    }
    buffer[0]='\0';
    n=0;
  }
  
  lcd.clear();
  if (received) {
    Serial.println(buffer);
    lcd.print(buffer);
  } else {
    Serial.println("timed out");
    lcd.print("timed out");
  }
  Serial.println("-------------");
  delay(250);
  lcd.clear();
  
  return ok;  
}

void work() {
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
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);
  while(!Serial);
  Altser.begin(9600);
  while (!sendCommand("AT"));
  while (!sendCommand("AT+RST"));
  while (!sendCommand("AT+UART_CUR=2400,8,1,0,0"));
  Altser.begin(2400);
  work();
}

int x=0;
int status=0;
int n=0;

void loop() {
  if (status==0) {
    while(!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.62\",8000"));
    
    char cmd[32]={0};
    snprintf(cmd, 16, "GET /set?x=%d HTTP/1.1\r\n\r\n", x++);

    char buffer[32]={0};
    snprintf(buffer, 16, "AT+CIPSEND=%d", strlen(cmd)+2);
    while(!sendCommand(buffer))
      delay(1000); // +2?
    
    while(!sendCommand(cmd));
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