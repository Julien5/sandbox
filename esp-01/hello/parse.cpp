#include "parse.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

/* We want to find if either
   (1) substr is contained in buffer at position 'startbuffer' OR if
   (2) the beginning of substr if contained at the end of buffer.
   * This function returns
   * 0 iff not (1) and not (2)
   * -1 if (1)
   * k>0 iff (2) and k is the first index of substr not contained in buffer.
   */
const int firstdiff(const char * buffer, const char * substr, int startbuffer, int startindex) {
  assert(startbuffer<int(strlen(buffer)));
  assert(startindex<int(strlen(substr)));
  if (startindex!=0 && startbuffer!=0) {
    // startindex not null
    // => startbuffer must be zero (otherwise retry from zero);
    return 0;
  }
  int l = 0;
  while(   buffer[startbuffer+l] != 0
	&& substr[startindex+l] != 0
	&& buffer[startbuffer+l]==substr[startindex+l]) {
    l++;
  }
  if (substr[startindex+l]==0) // end of substr => all found
    return -1;
  if (buffer[startbuffer+l]==0) // end of buffer => part found
    return startindex+l;
  return 0; // nothing found.
}

/* This function scans buffer for substr (starting at startindex).
   Return value => see above.
*/
int find(const char * buffer, const char * substr, int startindex=0) {
  const int Ls = strlen(substr);
  const int Lb = strlen(buffer);
  int p=0;
  int k=0;
  while(p<Lb && startindex<Ls && (k=firstdiff(buffer, substr, p, startindex))==0)
    p++;
  return k;
}

bool parse::StringAwaiter::read(const char * buffer) {
  first_not_found=find(buffer,wanted,first_not_found);
  if (first_not_found==-1) {
    first_not_found=0;
    return true;
  }
  return false;
}

void escape(char * buffer, char c) {
  for(int k=0; k<int(strlen(buffer)); ++k)
    if (buffer[k]==c)
      buffer[k]='_';
}


void parse::AccessPointParser::read(const char * _buffer) {
  // needs to much memory i think
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
	retain.append(p.c_str());
      }
    }
  }
  buffer.clear();
}

/*
 *
 +IPD,116:HTTP/1.0 200 OK
 Server: BaseHTTP/0.6 Python/3.4.2
 Date: Fri, 21 Dec 2018 23:08:34 GMT
 Content-type: text/html

 clock: 2019-01-05 23:52:44.735943 [end]

*/
#include "freememory.h"
void parse::TimeParser::read(const char * _buffer) {
  if (!in_frame && startAwaiter.read(_buffer))
    in_frame=true;
  if (!in_frame)
    return;

  if (!retain.empty()) {
    buffer.append(retain.c_str());
  }
  buffer.append(_buffer);
  debug(buffer);
  if (!buffer.contains("[end]"))
    return;
  
  buffer.zeroes(" .:\r\n");
  parse_index=0;
  while(char * _p = buffer.tok()) {
    nstring::STR<8> p(_p);
    if (p.contains("clock"))
      parse_index=0;
    parse_index++;
    debug(parse_index);
    debug(_p);
    debug("--");
    if (parse_index==3) {
      time[0] = char(p.toInt());
    }
    if (parse_index==4) {
      time[1] = char(p.toInt());
    }
    if (parse_index==5) {
      time[2] = char(p.toInt());
    }
    if (parse_index==7) {
      if (!p.contains("[end]")) {
	retain.clear();
	retain.append(p.c_str());
      }
    }
  }
  buffer.clear();
}

int accesspointparser_test() {
  using namespace parse;
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
    for(int k = 0; k<int(strlen(A)); ++k) {
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

int time_parser_test() {
  using namespace parse;
  {
    TimeParser p;
    p.read("+IPD,116:HTTP/1.0 200 OK\n");
    p.read("Server: BaseHTTP/0.6 Python/3.4.2\n");
    p.read("Date: Fri, 21 Dec 2018 23:08:34 GMT\n");
    p.read("Content-type: text/html\n\r");
    p.read("clock: 2019-01-05 23:52:44.735943 [end]");
    char * time = p.get();
    assert(time);
    assert(time[0]==23);
    assert(time[1]==52);
    assert(time[2]==44);
  }
  debug("time OK 1");
  {
    TimeParser p;
    p.read("erver: BaseHTTP");
    p.read("/0.6 Python/3.4");
    p.read(".2\nDate: Sat, ");
    p.read("05 Jan 2019 19:");
    p.read("08:46 GMT\nCont");
    p.read("ent-type: text/html\n");
    p.read("clock: 2019-01-0");
    p.read("5 23:52:44.73594");
    p.read("3 [end]\n");
    char * time = p.get();
    assert(time);
    assert(time[0]==23);
    assert(time[1]==52);
    assert(time[2]==44);
  }
  debug("time OK 6");
  return 0;
}


int parse_test() {
  using namespace parse;
  {
    assert(firstdiff("aabbcc","ab",0,0)==0);
    assert(firstdiff("aabbcc","ab",1,0)==-1);
    assert(firstdiff("aabbcc","ab",2,0)==0);
    assert(find("aabbcc","ab")==-1);
    assert(find("aabbcc","ccc")==2);
    assert(find("cx","ccc",2)==-1);
    assert(find("cx","ccc")==0);
  }
  
  {
    assert(find("aabbcc","ab")==-1);
    assert(find("aabbcc","ccc")==2);
    assert(find("cx","ccc",2)==-1);
    assert(find("cx","ccc")==0);
  }
  
  {
    StringAwaiter a("OK");
    assert(a.read("OK"));
    assert(a.read("aaaOKaaa"));
    assert(!a.read("buuuh"));
    assert(!a.read("buuuO"));
    assert(a.read("KO"));
    assert(!a.read("buuuO"));
    assert(!a.read("xKO"));
  }
  
  {
    StringAwaiter a("1234567890");
    assert(!a.read("---12"));
    assert(!a.read("3456"));
    assert(a.read("7890ssss"));  
  }
  
  {
    debug("*******");
    StringAwaiter a("OK");
    assert(a.read("OXOK"));
  }

  {
    debug("*******");
    StringAwaiter a("ERROR");
    assert(!a.read("ERR"));
    assert(a.read("OR\n"));
  }

  {
    debug("*******");
    StringAwaiter a("OK");
    assert(!a.read(",\"7"));
    assert(!a.read("c:f"));
    assert(!a.read("f:4"));
    assert(!a.read("d:c"));
    assert(!a.read("4:6"));
    assert(!a.read("b:f"));
    assert(!a.read("3\","));
    assert(!a.read("11,"));
    assert(!a.read("130"));
    assert(!a.read(",0)"));
    assert(!a.read("\n"));
    assert(a.read("OK"));
  }
  return 0;
}


int parse::test() {
  int ret=0;
  ret=parse_test();
  if (ret!=0)
    return ret;
  ret=accesspointparser_test();
  if (ret!=0)
    return ret;
  ret=time_parser_test();
  if (ret!=0)
    return ret;
  return ret;
}
