#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 5, 6, 10, 11, 12);

#ifndef HARDWARE_SERIAL
#include "AltSoftSerial.h"
#endif

#include "parse.h"
#include "wifi.h"

void print(String msg) {
  Serial.println(msg);
}

void display(char * msg) {
  lcd.clear();
  lcd.print(msg);
}

void display(const char * msg) {
  lcd.clear();
  lcd.print(msg);
}

wifi::esp8266 esp;

void setup()
{
  lcd.begin(16, 2);
  lcd.clear();
  
  for(int d=0; d<3; ++d)
  {
    char msg[16]={0};
    snprintf(msg, 16, "INIT LCD: %d", d);    
    display(msg);
    delay(500);
  }

  display("init serial");
  Serial.begin(9600);
  while(!Serial);

  display("init soft.serial");
 

  display("init ESP");
  delay(250);
  esp.reset();
  /*
  while (!sendCommand("AT"));
  while (!sendCommand("AT+RST"));
  while (!sendCommand("AT+UART_CUR=2400,8,1,0,0"));
  Altser.begin(2400);while(Altser.available()) Serial.print(Altser.read());
  delay(250);
  while(!sendCommand("AT+CWLAP"));
  while(!sendCommand("AT+CWJAP_CUR=\"JBO\",\"00000000001111111111123456\""))
    delay(1000);
  while(!sendCommand("AT+CWJAP_CUR?"));
  while(!sendCommand("AT+PING=\"192.168.2.62\""));
  while(!sendCommand("AT+CIPMUX?"));
  while(!sendCommand("AT+CIPMODE?"));
  */
  display("init done. good.");
  delay(1000);
}

int x=0;
int status=0;
int n=0;


void loop() {
  /*
  if (status==0) {
    if (n==0)
      sendCommand("AT+CWLAP");
    while(!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.2.62\",8000"));
   
    char cmd[128]={0};
    snprintf(cmd, 128, "GET /set?name=%s&rssi=%d HTTP/1.1\r\n\r\n", app.get(n).name, app.get(n).rssi);
    
    Serial.print(cmd);
    
    char buffer[32]={0};
    snprintf(buffer, 32, "AT+CIPSEND=%d", strlen(cmd)+2);

    Serial.print(buffer);
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
    if (n++>=app.size())
      n=0;
    Serial.print("done");
    sendCommand("AT+CIPCLOSE");
    delay(250);
    status=0;
  }
  */
}