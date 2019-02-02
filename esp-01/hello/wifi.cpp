#include "wifi.h"
#include "parse.h"
#include <stdlib.h>
#include "lcd.h"
#include "freememory.h"
#include "clock.h"
#include "debug.h"

#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define ESPTX Altser
#define ESPRX Altser

#define BUFFER_LENGTH 16

// see https://www.nongnu.org/avr-libc/user-manual/pgmspace.html
const char ATCWQAP[] = "AT+CWQAP";
const char ATRST[] = "AT+RST";
const char ATCWJAP[] = "AT+CWJAP?";
const char ATE1[] = "ATE1";
const char AT[] = "AT";
const char ATCWLAP[] = "AT+CWLAP";
const char ATCIPSTART[] = "AT+CIPSTART";
const char ATCIPSEND[] = "AT+CIPSEND";
const char ATCIPCLOSE[] = "AT+CIPCLOSE";
const char UARTCUR[] = "UART_CUR";

const int short_timeout = 3000;
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
  const unsigned char wait_for_connected = 0x10;
}

void flushRX() {
  ESPRX.flushInput();
  ESPTX.flushInput();
}

parse::MessageParser message_parser;

unsigned char waitFor(const unsigned char opts, const int timeout) {
  flushRX();
  unsigned long now = millis();
  unsigned long deadline = now + timeout;
   
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");
  parse::StringAwaiter closed_wait("CLOSED");
  parse::StringAwaiter gt_wait(">");
  parse::StringAwaiter connected_wait("CONNECTED");

  unsigned char found = 0;
  
  delay(250);
  TRACE();
  DBGTX("waitfor[");
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
      if (options::wait_for_connected & opts) {
	if (connected_wait.read(buffer))
	  found |= options::wait_for_connected;
      }
      message_parser.read(buffer);
    }
    buffer[0]='\0';
  }
  DBGTX("message={");DBGTX(message_parser.get());DBGTX("}");
  
  TRACE();
  DBGTX("]...");
  DBGTX("\r\n*\r\n*\r\n");
  return found;  
}

unsigned char sendCommandAndWaitForResponse(const char * command, const int length, const int timeout)
{
  comm::write(command,length);
  comm::write("\r\n");
  DBGTX("command: ");
  DBGTXLN(command);
  if (strstr(command,UARTCUR)!=NULL) {
    delay(150);
    return options::wait_for_ok;
  }
  unsigned char opts=options::wait_for_ok | options::wait_for_error;
  if (strstr(command,ATCIPSEND)!=NULL)
    opts |= options::wait_for_gt;
  if (strstr(command,ATRST)!=NULL)
    opts |= options::wait_for_connected;
  TRACE();
  unsigned char ret=waitFor(opts, timeout);
  DBGTX("sendCommandAndWaitForResponse: ");
  DBGTXLN(int(ret));
  return ret;
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
  delay(250);
  m_enabled=enable();
}

bool wifi::esp8266::enabled() const {
  return m_enabled;
}

wifi::esp8266::~esp8266() {
  sendCommandAndWaitForResponse(ATCWQAP,short_timeout);
  disable();
}

bool wifi::esp8266::enable() {
  DBGTX("wifi up");
  digitalWrite(enable_pin, HIGH);
  delay(250);
  if (!reset())
    return false;
  delay(5000);
  if (!join())
    return false;
  return true;
}

void wifi::esp8266::disable() {
  DBGTX("wifi down");
  digitalWrite(enable_pin, LOW);
}

void wifi::esp8266::setTimeout(int t) {
  timeout=t;
}

namespace command {
  bool RST() {
    uint8_t ret = sendCommandAndWaitForResponse(ATRST,short_timeout);
    return ret==options::wait_for_ok || ret==options::wait_for_connected;
  }
};

bool wifi::esp8266::reset() {
  resetSerial();

  digitalWrite(enable_pin,LOW);
  delay(250);
  digitalWrite(enable_pin, HIGH);
  delay(250);
  
  char trial=16;
  while(!command::RST()) {
    delay(150);
    trial--;
    if (trial<=0)
      return false;
  }
  return true;
}

bool wifi::esp8266::join() {
  if (sendCommandAndWaitForResponse(ATCWJAP,short_timeout)==options::wait_for_ok)
    m_joined=true;
  
  if (!m_joined) {
    sendCommandAndWaitForResponse(ATE1,short_timeout);
    sendCommandAndWaitForResponse(AT,short_timeout);
    //while(!sendCommandAndWaitForResponse("AT+CWLAP",long_timeout)) {
    //  AT::RST();
    //  delay(250);
    //}
    m_joined=sendCommandAndWaitForResponse("AT+CWJAP_CUR=\"JBO\",\"7981409790562366\"",long_timeout)
      ==options::wait_for_ok;
  }
  return m_joined;
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
  bool m_opened=false;
  bool open() {
    return sendCommandAndWaitForResponse("AT+CIPSTART=\"TCP\",\"192.168.178.24\",8000",long_timeout)
      &options::wait_for_ok != 0;
  }
  bool close() {
    return sendCommandAndWaitForResponse(ATCIPCLOSE,short_timeout)
      &options::wait_for_ok != 0;
  }
public:
  IPConnection() {
    //  close(); // ignore the result.
    m_opened=open();
  }
  bool opened() {
    return m_opened;
  }
  ~IPConnection() {
    // if (m_opened)
      close();
  }
};

bool wifi::esp8266::get(const char * req, char** response) {
  DBGTXLN("-- GET --");
  Slower slower;
  IPConnection connection;
  if (!connection.opened())
    return false;
  
  const char request[128]={0};
  snprintf(request, 128, "GET /%s HTTP/1.1\r\n\r\n", req);
  
  const char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", strlen(request)+2);
  
  const char ok=sendCommandAndWaitForResponse(cipsend,short_timeout);
  if (ok&options::wait_for_ok == 0)
    return false;
  
  sendCommandAndWaitForResponse(request,0);

  message_parser.reset();
  if (!waitFor(options::wait_for_closed,long_timeout)) {
    return false;
  }
  
  *response = message_parser.get();
  DBGTX("*response={");DBGTX(*response);DBGTXLN("}");
  return true;
}

int wifi::esp8266::post(const char * req, const uint8_t * data, const int Ldata, char** response) {
  //while (!ping()) {
  //  DBGTXLN("server unreachable");
  //  delay(1000);
  //}
  IPConnection connection;
  if (!connection.opened())
    return 1;
  
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
  char trials=1;
  while(trials>0 && !(ok=sendCommandAndWaitForResponse(cipsend,short_timeout))) {
    trials--;
    delay(1000);
  }
  if (ok&options::wait_for_ok == 0)
    return 3;

  DBGTXLN(request);
  DBGTX("+");
  DBGTX(Ldata);
  DBGTXLN(" data bytes");
  comm::write(request,strlen(request));
  comm::write((char*)data,Ldata);
  comm::write("\r\n");

  message_parser.reset();
  if (!waitFor(options::wait_for_ok | options::wait_for_error, long_timeout))
    return 4;

  *response = message_parser.get();
  return 0;
}


int wifi::test() {
  return 0;
}
