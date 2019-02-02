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
  first_not_found=find(buffer,m_wanted,first_not_found);
  if (first_not_found==-1) {
    first_not_found=0;
    return true;
  }
  return false;
}

const char * parse::StringAwaiter::wanted() const {
  return m_wanted;
}

void parse::MessageParser::reset() {
  state = 0;
  memset(message,0,sizeof(message));
}

void parse::MessageParser::read(char* buffer) {
  if (state == 2)
    return;
  
  char *begin=buffer;
  char *end=buffer+strlen(buffer);
 
  if (state == 0 && startAwaiter.read(buffer)) {
    state=1;
    begin=strstr(buffer,startAwaiter.wanted());
    assert(begin);
    begin += strlen(startAwaiter.wanted());
  }

  if (state == 1 && endAwaiter.read(buffer)) {
    state=2;
    end=strstr(buffer,endAwaiter.wanted());
    assert(end);
  }
  if (state==0) 
    return;

  strncat(message,begin,end-begin);

  if (state==2) {
    message[sizeof(message)-1]=0;
  }
}

int message_test() {
  parse::MessageParser m;
  m.read("blahahahaah ");
  assert(!m.get());
  m.read("blah {");
  assert(!m.get());
  m.read("jj");
  assert(!m.get());
  m.read("}  d ");
  assert(m.get());
  debug(m.get());
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

char pmessage[16];
bool pget(char ** m) {
  snprintf(pmessage,16,"%u for %02d:%02d",1,2,3);
  debug(pmessage);
  *m=pmessage;
  debug(*m);
  assert(*m);
  return true;
}
#include <string.h>
#include <stdlib.h>
int smoke() {
  char * m=0;
  if (!pget(&m))
      return 0;
  debug(m);
  assert(m);
  assert(strlen(m));
  printf("XX[%-.16s]\n","A23456789B123456XXXXXXXX");
  printf("XX[%-16.16s]\n","A234567");
  printf("XX%-.2d\n",456);
  return 0;
}

int parse::test() {
  int ret=0;
  ret=parse_test();
  if (ret!=0)
    return ret;

  ret=message_test();
  if (ret!=0)
    return ret;

  ret=smoke();
  if (ret!=0)
    return ret;
  
  return ret;
}
