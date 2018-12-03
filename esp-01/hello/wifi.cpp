#include "wifi.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

void escape(char * buffer, char c) {
  for(int k=0; k<strlen(buffer); ++k)
    if (buffer[k]==c)
      buffer[k]='_';
}


void wifi::AccessPointParser::read(const char * _buffer) {
  if (!retain.empty()) {
    buffer.append(retain.c_str());
  }
  buffer.append(_buffer);
  if (!buffer.contains(')'))
    return;

  debug(buffer);
  if (buffer.contains("AT")) {
    N=0;
  }
  debug(N);
  
  buffer.zeroes("(),\r\n+");
  while(char * _p = buffer.tok()) {
    debug(_p);
    nstring::STR<32> p(_p);
    if (p=="CWLAP:") {
      N++;
      debug(N);
      parse_index=0;
    }
    parse_index++;
    debug(parse_index);
    if (parse_index==3) {
      p.replace(' ','_');
      // remove char " in "JBO"
      int Lp = p.size();
      strncpy(m_aps[N-1].name,_p+1,15);
      m_aps[N-1].name[Lp-2]='\0';
    }
    if (parse_index==4) {
      m_aps[N-1].rssi=p.toInt();
    }
    if (parse_index==9) {
      if (!p.contains("OK")) { // strcmp(p,"OK") != 0) {// (1), "p does not contain OK"
	retain.clear();
	retain.append(_p);
      }
    }
  }
  buffer.clear();
}

int wifi::test() {
  /*
    AT+CWLAP
    +CWLAP:(1,"JBO",-72,"00:1a:4f:00:41:92",2,101,0)
    +CWLAP:(3,"FRITZ!Box 7430 TJ",-70,"7c:ff:4d:c4:6b:f3",11,127,0)
  */
  {
    AccessPointParser p;
    p.read("AT+CWLAP\n");
    p.read("+CWLAP:(1,\"JBO\",-72,\"00:1a:4f:00:41:92\",2,");
    p.read("101,0)\n+CWL");
    p.read("AP:(3,\"FRITZ!Box 7430 TJ\",-70,\"7c");
    p.read(":ff:4d:c4:6b:f3\",11,127,0)");
    assert(p.size()==2);
    for(int k = 0; k<p.size(); ++k) {
      debug(p.get(k).name);
      debug(p.get(k).rssi);
    }    
  }
  debug("****1*****");
  {
    AccessPointParser p;
    const char * A = "AT+CWLAP\n+CWLAP:(1,\"JBO\",-72,\"00:1a:4f:00:41:92\",2,101,0)";
    for(int k = 0; k<strlen(A); ++k) {
      char c[2];
      c[0]=A[k];
      c[1]='\0';      
      p.read(c);
    }
    assert(p.size()==1);
  }
  debug("*****2****");
  {
    AccessPointParser p;
    for(int k=0; k<2; ++k) {
      p.read("\r\nAT+CWLAP\r\r\n");
      p.read("+CWLAP:(1,\"JBO\",-72,\"00:1a:4f:00:41:92\",2,101,0)\n");
      p.read("+CWLAP:(1,\"foo\",72,\"00:1a:4f:00:41:92\",2,101,0)\n");
      p.read("+CWLAP:(1,\"JBObar\",-272,\"00:1a:4f:00:41:92\",2,101,0)\n");
      p.read("\n");
      p.read("OK\r\n");
    }
    assert(p.size()==3);
    for(int k = 0; k<p.size(); ++k) {
      debug(p.get(k).name);
      debug(p.get(k).rssi);
    }    
    assert(std::string(p.get(0).name)=="JBO");
  }
  return 0;
}
