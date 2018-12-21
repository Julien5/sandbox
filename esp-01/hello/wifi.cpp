#include "wifi.h"
#include "parse.h"
#include "nstring.h"
#include "lcd.h"
#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define ESPTX Altser
#define ESPRX Altser
#define DBGTX Serial

#define L 15

bool waitForResponse() {
  parse::StringAwaiter a("CLOSED");
  long int now = millis();
  long unsigned int deadline = now + 5000;
  char buffer[L+1]={0};
  display::lcd.print("[wifi rx]");
  while(millis()<deadline) {
    while(ESPRX.available()) {
      int n=ESPRX.readBytes(buffer,min(ESPRX.available(),L));
      buffer[n]='\0';
      DBGTX.write(buffer);
      if (a.read(buffer))
	return true; 
    }
  }
  return false;
}

void clearbuffer() {
  //ESPRX.flushInput();
  //ESPRX.flushOutput();
  ESPTX.flushInput();
  ESPTX.flushOutput();
}


boolean sendCommand(const char * command, const int length, const int timeout)
{
  ESPTX.write(command,length);
  ESPTX.write("\r\n");
  DBGTX.println(command);
  
  if (strstr(command,"UART_CUR")!=NULL) {
    delay(150);
    return true;
  }

  long unsigned int now = millis();
  long unsigned int deadline = now + timeout;

  bool received=false;
  bool ok=false;
  bool error=false;

  char buffer[L+1]={0};
  
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");

  display::lcd.print(command);
  delay(250);
  DBGTX.write("[");
  while(millis()<deadline && !received) {
    while(ESPRX.available() && !received) {
      int n=ESPRX.readBytes(buffer,min(ESPRX.available(),L));
      buffer[n]='\0';
      DBGTX.write(buffer);
      ok=ok_wait.read(buffer);
      error=error_wait.read(buffer);
      received = ok || error;
    }
    buffer[0]='\0';
  }
  DBGTX.write("]...");

  if (timeout<=1000)
     deadline = now + 250;
  else
    deadline = now + 1000;
  while(millis()<deadline && !received) {
    while(ESPRX.available() && !received) {
      int n=ESPRX.readBytes(buffer,min(ESPRX.available(),L));
      buffer[n]='\0';
      DBGTX.write(buffer);
    }
  }
  DBGTX.write("*\r\n*\r\n*\r\n");
  return ok;  
}

boolean sendCommand(const char * command, const int timeout) {
  return sendCommand(command,strlen(command),timeout);
}

const int short_timeout = 1000;
const int long_timeout = 20000;

void resetSerial() {
  ESPRX.begin(9600);
  ESPTX.begin(9600);
  clearbuffer();
}

wifi::esp8266::esp8266(char pin)
  : timeout(3000)
  , enable_pin(pin) {
  resetSerial();
  pinMode(enable_pin, OUTPUT);
  enable();
}

wifi::esp8266::~esp8266() {
  disable();
}

void wifi::esp8266::enable() {
  digitalWrite(enable_pin, HIGH);
  delay(250);
  reset();
  join();
}

void wifi::esp8266::disable() {
  digitalWrite(enable_pin, LOW);
}

void wifi::esp8266::setTimeout(int t) {
  timeout=t;
}

bool wifi::esp8266::reset() {
  resetSerial();
  char trial=16;
  while(trial>=0 && !sendCommand("AT+RST",short_timeout)) {
    trial--;
  }
  return sendCommand("AT+RST",short_timeout);
}

bool wifi::esp8266::join() {
  sendCommand("ATE1",short_timeout);
  sendCommand("AT",short_timeout);
  while(!sendCommand("AT+CWLAP",long_timeout)) {
    sendCommand("AT+RST",short_timeout);
    delay(250);
  }
  return sendCommand("AT+CWJAP_CUR=\"JBO\",\"7981409790562366\"",long_timeout);
}

bool wifi::esp8266::ping() {
  return sendCommand("AT+PING=\"192.168.178.24\"",long_timeout);
}

class Slower {
public:
  Slower() {
    while(!sendCommand("AT+UART_CUR=2400,8,1,0,0",short_timeout));
    ESPRX.begin(2400);
    ESPTX.begin(2400);
  }
  ~Slower() {
    while(!sendCommand("AT+UART_CUR=9600,8,1,0,0",short_timeout));
    ESPRX.begin(9600);
    ESPTX.begin(9600);
  }  
};

bool wifi::esp8266::get(const char * req) {
  Slower slower;
  if (!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.178.24\",8000",long_timeout))
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

int wifi::esp8266::post(const char * req, const char * data, const int Ldata) {
  ESPTX.print("** upload "); DBGTX.print(Ldata); DBGTX.println(" bytes");
  while (!ping()) {
    DBGTX.println("server unreachable");
    delay(250);
  }
  sendCommand("AT+CIPCLOSE",short_timeout); // ignore the result.
  if (!sendCommand("AT+CIPSTART=\"TCP\",\"192.168.178.24\",8000",long_timeout))
    return 1;

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
	   "Host: 192.168.178.24",
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
    return 2;

  if (!sendCommand(request,Lr,long_timeout))
    return 3;
  
  if (!waitForResponse())
    return 4;

  sendCommand("AT+CIPCLOSE",short_timeout);
  return 0;
}


int wifi::test() {
  return 0;
}
