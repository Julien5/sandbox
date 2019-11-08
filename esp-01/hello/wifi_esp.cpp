#include "wifi_esp.h"
#include "parse.h"
#include <stdlib.h>
#include "lcd.h"
#include "freememory.h"
#include "clock.h"
#include "debug.h"
#include "platform.h"

#ifdef ARDUINO
#include "AltSoftSerial.h"
AltSoftSerial Altser;
#define ESPTX Altser
#define ESPRX Altser
#else
#include "platform.h"
#define ESPTX Serial
#define ESPRX Serial
#endif

#define BUFFER_LENGTH 16

// see https://www.nongnu.org/avr-libc/user-manual/pgmspace.html

const char ATCIPSEND[] = "AT+CIPSEND";
const char ATCWQAP[] = "AT+CWQAP";
const char ATCIPCLOSE[] = "AT+CIPCLOSE";
const char UARTCUR[] = "UART_CUR";
const char ATCWJAP[] = "AT+CWJAP?";
const char ATE1[] = "ATE1";
const char AT[] = "AT";
const char ATRST[] = "AT+RST";

const int short_timeout = 3000;
const int long_timeout = 20000;

namespace comm {
  int available() {
    return ESPRX.available();
  }
  
  int readBytes(char *buffer, const int length) {
    return ESPRX.readBytes(buffer,length);
  }
  
  int write(const uint8_t *buffer, int length=-1) {
    if (length>=0)
      return ESPTX.write((const char*)buffer,length);
    return ESPTX.write((const char*)buffer);
  }
  
  int write(const char *buffer, int length=-1) {
    if (length>=0)
      return ESPTX.write(buffer,length);
    return ESPTX.write(buffer);
  }
}

namespace options {
  const unsigned char wait_for_ok        = 0b00000001; 
  const unsigned char wait_for_error     = 0b00000010;
  const unsigned char wait_for_closed    = 0b00000100;
  const unsigned char wait_for_gt        = 0b00001000;
  const unsigned char wait_for_connected = 0b00010000;
  const unsigned char wait_for_no_ap     = 0b00100000;
}

void flushRX() {
  ESPRX.flushInput();
  ESPTX.flushInput();
}

parse::MessageParser message_parser;

unsigned char waitFor(const unsigned char opts, const int timeout) {
  /*
   * This function is at the core of the whole stuff.
   * Be careful, in particular w.r.t. memory usage.
   */
  
  flushRX();
  unsigned long now = millis();
  unsigned long deadline = now + timeout;
   
  parse::StringAwaiter ok_wait("OK");
  parse::StringAwaiter error_wait("ERROR");
  parse::StringAwaiter closed_wait("CLOSED");
  parse::StringAwaiter gt_wait(">");
  parse::StringAwaiter connected_wait("CONNECTED");
  parse::StringAwaiter no_ap_wait("No AP");

  unsigned char found = 0;
  
  delay(250);
  TRACE();
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
      if (options::wait_for_connected & opts) {
	if (connected_wait.read(buffer))
	  found |= options::wait_for_connected;
      }
      if (options::wait_for_no_ap & opts) {
	if (no_ap_wait.read(buffer))
	  found |= options::wait_for_no_ap;
      }
      message_parser.read(buffer);
    }
    buffer[0]='\0';
  }
  if (message_parser.get())
    DBGTX("message={");DBGTX(message_parser.get());DBGTX("}");
  
  TRACE();
  DBGTX("]\r\n\r\n");
  return found;  
}

unsigned char sendCommandAndWaitForResponse(const char * command, const int length, const int timeout)
{
  comm::write(command,length);
  comm::write("\r\n");
  DBGTX("C:");DBGTXLN(command);
  if (strstr(command,UARTCUR)!=NULL) {
    delay(150);
    return options::wait_for_ok;
  }
  unsigned char opts=options::wait_for_ok | options::wait_for_error;
  if (strstr(command,ATCIPSEND)!=NULL)
    opts |= options::wait_for_gt;
  if (strstr(command,ATRST)!=NULL)
    opts |= options::wait_for_connected;
  if (strstr(command,ATCWJAP)!=NULL)
    opts |= options::wait_for_no_ap;
  unsigned char ret=waitFor(opts, timeout);
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

const int wifi_enable_pin = 3;
wifi::esp8266::esp8266()
  : timeout(3000)
  , enable_pin(wifi_enable_pin) {
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
  
  char trial=3;
  while(!command::RST()) {
    delay(150);
    trial--;
    if (trial<=0)
      return false;
  }
  return true;
}

bool wifi::esp8266::join() {
  const unsigned char cwjap_response=sendCommandAndWaitForResponse(ATCWJAP,short_timeout);
  if (cwjap_response&options::wait_for_ok)
    m_joined=false;
  if (cwjap_response&options::wait_for_no_ap)
    m_joined=false;
  if (!m_joined) {
    sendCommandAndWaitForResponse(ATE1,short_timeout);
    sendCommandAndWaitForResponse(AT,short_timeout);
    sendCommandAndWaitForResponse("AT+CWMODE=1",short_timeout);
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
  return sendCommandAndWaitForResponse("AT+PING=\"pi.fritz.box\"",short_timeout);
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

static long long last_connection_time=0;
class IPConnection {
  bool m_opened=false;
  bool open() {
    // it seems esp8266 does not like too close CIPSTART..
    // => wait a little if the last CIPSTART is recent.
    if ((millis() - last_connection_time)<1000)
      delay(1000);
    return (sendCommandAndWaitForResponse("AT+CIPSTART=\"TCP\",\"pi.fritz.box\",8000",long_timeout)
	    &options::wait_for_ok) != 0;
  }
  bool close() {
    last_connection_time=millis();
    return (sendCommandAndWaitForResponse(ATCIPCLOSE,short_timeout)
	    &options::wait_for_ok) != 0;
  }
public:
  IPConnection() {
    m_opened=open();
  }
  bool opened() {
    return m_opened;
  }
  ~IPConnection() {
    close();
  }
};

bool wifi::esp8266::get(const char * req, char** response) {
  // i dont know why, but this delay is necessary for multiple, close GET requests.
  delay(2000);
  Slower slower;
  IPConnection connection;
  if (!connection.opened())
    return false;
  
  char request[128]={0};
  snprintf(request, 128, "GET /%s HTTP/1.1\r\n\r\n", req);
  
  char cipsend[32]={0};
  snprintf(cipsend, 32, "AT+CIPSEND=%d", strlen(request)+2);
  
  const char ok=sendCommandAndWaitForResponse(cipsend,short_timeout);
  if ((ok&options::wait_for_ok) == 0)
    return false;
  
  sendCommandAndWaitForResponse(request,0);

  message_parser.reset();
  if (!waitFor(options::wait_for_closed,long_timeout)) {
    return false;
  }
  assert(response);  
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
	   "Host: pi.fritz.box",
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
  while(trials>0 && !(ok=sendCommandAndWaitForResponse(cipsend,long_timeout))) {
    trials--;
    delay(1000);
  }
  if ((ok&options::wait_for_ok) == 0)
    return 3;

  comm::write(request,strlen(request));
  comm::write((char*)data,Ldata);
  comm::write("\r\n");

  message_parser.reset();
  if (!waitFor(options::wait_for_ok | options::wait_for_error, long_timeout))
    return 4;

  if (response)
    *response = message_parser.get();
  return 0;
}
