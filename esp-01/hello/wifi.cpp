#include "wifi.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

void escape(char * buffer, char c) {
  for(int k=0; k<strlen(buffer); ++k)
    if (buffer[k]==c)
      buffer[k]='_';
}


void wifi::AccessPointParser::read(char * _buffer) {
  if (strlen(retain)>0) {
    strcat(buffer,retain);
    retain[0]='\0';
  }
  if (false && strchr(_buffer,'\r')) {
    int L=strcspn(_buffer,"\r");
    strncat(buffer,_buffer,L);
    strcat(buffer,"\n");
  } else
    strcat(buffer,_buffer);
  
  if (!strchr(buffer,')'))
    return;

  char * p=strtok(buffer,"(,\r\n");
  while(p) {
    if (strstr(p,"AT+CWLAP")) {
      // fuzzy compare, p contains sometimes garbage.
      // (1) not enough.
      N=-1;
      parse_index=0;
    }
    if (!strcmp(p,"+CWLAP:")) {
      N++;
      parse_index=1;
    }
    else
      parse_index++;
    
    if (parse_index==3) {
      escape(p,' ');
      debug(p);
      // remove char " in "JBO"
      int Lp = strlen(p);
      strncpy(m_aps[N].name,p+1,15);
      m_aps[N].name[Lp-2]='\0';
    }
    if (parse_index==4) {
      m_aps[N].rssi=atoi(p);
    }
    if (parse_index==9) {
      if (strcmp(p,"OK")) // (1)
	strncpy(retain,p,15);
    }
    assert(parse_index<=9);
    p=strtok(NULL,"(,\r\n");
  }
  buffer[0]='\0';
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
  debug("*********");
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
  debug("*********");
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
