#include "wifi.h"
#include "parse.h"
#include "nstring.h"
#include "lcd.h"
#include "freememory.h"

#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define ESPTX Altser
#define ESPRX Altser

#define BUFFER_LENGTH 16

const int short_timeout = 1000;
const int long_timeout = 20000;

namespace comm {
  int available() {
    //return 0;
    return ESPRX.available();
  }
  
  int readBytes(char *buffer, const int length) {
    return ESPRX.readBytes(buffer,length);
  }
  
  int write(const uint8_t *buffer, int length=-1) {
    if (length>=0)
      return ESPTX.write(buffer,length);
    return ESPTX.write((const char*)buffer);
  }
  
  int write(const char *buffer, int length=-1) {
    if (length>=0)
      return ESPTX.write((const char*)buffer,length);
    return ESPTX.write(buffer);
  }
}

namespace options {
  const unsigned char wait_for_ok = 0x1; 
  const unsigned char wait_for_error = 0x2;
  const unsigned char wait_for_closed = 0x4;
  const unsigned char wait_for_gt = 0x8;
}

void flushRX() {
  ESPRX.flushInput();
  ESPTX.flushInput();
}

unsigned char waitFor(const unsigned char opts, const int timeout, parse::TimeParser *time_parser=nullptr) {
  flushRX();
  unsigned long now = millis();
  unsigned long deadline = now + timeout;
   
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");
  parse::StringAwaiter closed_wait("CLOSED");
  parse::StringAwaiter gt_wait(">");

  unsigned char found = 0;
  
  delay(250);
  printMemory(127);
  DBGTX("[");
  char buffer[BUFFER_LENGTH]={0};
  while(millis()<deadline && !found) {
    while(comm::available() && !found) {
      int n=comm::readBytes(buffer,min(comm::available(),BUFFER_LENGTH-1));
      buffer[n]='\0';
      DBGTX(buffer);
      if (options::wait_for_ok & opts) {
	if (ok_wait.read(buffer))
	  found |= options::wait_for_ok;
      }
      if (options::wait_for_error & opts) {
	if (error_wait.read(buffer))
	  found |= options::wait_for_error;
      }
      if (options::wait_for_closed & opts) {
	if (closed_wait.read(buffer))
	  found |= options::wait_for_closed;
      }
      if (options::wait_for_gt & opts) {
	if (gt_wait.read(buffer))
	  found |= options::wait_for_gt;
      }
      //if (time_parser)
      //time_parser->read(buffer);
    }
    buffer[0]='\0';
  }
  DBGTX("]...");
  DBGTX("\r\n*\r\n*\r\n");
  return found;  
}

unsigned char sendCommandAndWaitForResponse(const char * command, const int length, const int timeout)
{
  comm::write(command,length);
  comm::write("\r\n");
  display::lcd.print(command);
  DBGTXLN(command);
  if (strstr(command,"UART_CUR")!=NULL) {
    delay(150);
    return true;
  }
  unsigned char opts=options::wait_for_ok | options::wait_for_error;
  if (strstr(command,"CIPSEND")!=NULL)
    opts |= options::wait_for_gt;
  return waitFor(opts, timeout);
}

unsigned char sendCommandAndWaitForResponse(const char * command, const int timeout) {
  return sendCommandAndWaitForResponse(command,strlen(command),timeout);
}

void resetSerial() {
  ESPRX.begin(9600);
  ESPTX.begin(9600);
  flushRX();
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
  DBGTX("wifi wake up");
  digitalWrite(enable_pin, HIGH);
  delay(250);
  reset();
  join();
}

void wifi::esp8266::disable() {
  DBGTX("wifi sleep");
  digitalWrite(enable_pin, LOW);
}

void wifi::esp8266::setTimeout(int t) {
  timeout=t;
}

namespace AT {
  bool RST() {
    return sendCommandAndWaitForResponse("AT+RST",short_timeout);
  }
};

bool wifi::esp8266::reset() {
  resetSerial();
  char trial=16;
  while(trial>=0 && !AT::RST()) {
    disable();
    delay(50);
    enable();
    trial--;
  }
  return AT::RST();
}

bool wifi::esp8266::join() {
  sendCommandAndWaitForResponse("ATE1",short_timeout);
  sendCommandAndWaitForResponse("AT",short_timeout);
  while(!sendCommandAndWaitForResponse("AT+CWLAP",long_timeout)) {
    AT::RST();
    delay(250);
  }
  return sendCommandAndWaitForResponse("AT+CWJAP_CUR=\"JBO\",\"7981409790562366\"",long_timeout);
}

bool wifi::esp8266::ping() {
  return sendCommandAndWaitForResponse("AT+PING=\"192.168.178.24\"",short_timeout);
}

class Slower {
public:
  Slower() {
    while(!sendCommandAndWaitForResponse("AT+UART_CUR=2400,8,1,0,0",short_timeout));
    delay(150);
    ESPRX.begin(2400);
    ESPTX.begin(2400);
    flushRX();
    
  }
  ~Slower() {
    while(!sendCommandAndWaitForResponse("AT+UART_CUR=9600,8,1,0,0",short_timeout));
    ESPRX.begin(9600);
    ESPTX.begin(9600);
  }  
};

class IPConnection {
  bool opened=false;
  bool open() {
    return sendCommandAndWaitForResponse("AT+CIPSTART=\"TCP\",\"192.168.178.24\",8000",long_timeout);
  }
  bool close() {
    return sendCommandAndWaitForResponse("AT+CIPCLOSE",short_timeout);
  }
public:
  IPConnection() {
    close(); // ignore the result.
    opened=open();
  }
  ~IPConnection() {
    // if (opened)
      close();
  }
};

bool wifi::esp8266::get(const char * req) {
  Slower slower;
  IPConnection connection;
  
  char request[128]={0};
  snprintf(request, 128, "GET %s HTTP/1.1\r\n\r\n", req);
  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", strlen(request)+2);
    
  if (!sendCommandAndWaitForResponse(cipsend,short_timeout))
    return false;
  
  if (!sendCommandAndWaitForResponse(request,short_timeout))
    return false;

  if (!waitFor(options::wait_for_closed,long_timeout,&time_parser)) {
    return false;
  }
  return true;
}

bool wifi::esp8266::get_time(char *h, char *m, char *s) {
  char * T = time_parser.get();
  if (!T)
    return false;
  *h = T[0];
  *m = T[1];
  *s = T[2];
  return true;
}

int wifi::esp8266::post(const char * req, const uint8_t * data, const int Ldata) {
  //while (!ping()) {
  //  DBGTXLN("server unreachable");
  //  delay(1000);
  //}
  IPConnection connection;

  char contentlength[32]={0};
  snprintf(contentlength, 32, "Content-Length: %d", Ldata);

  char request[128]={0};
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
    ;
  const int Lr = k;
  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", Lr+2);

  unsigned char ok=0;
  char trials=3;
  while(trials-->0 && !(ok=sendCommandAndWaitForResponse(cipsend,short_timeout)))
    delay(1000);
  if (ok==0)
    return 3;

  DBGTXLN(request);
  DBGTX("+");
  DBGTX(Ldata);
  DBGTXLN(" data bytes");
  comm::write(request,strlen(request));
  comm::write((char*)data,Ldata);
  comm::write("\r\n");
  
  if (!waitFor(options::wait_for_ok | options::wait_for_error, long_timeout))
    return 4;

  return 0;
}


int wifi::test() {
  return 0;
}
