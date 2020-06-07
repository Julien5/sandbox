#include "common/stringawaiter.h"

#include "common/debug.h"
#include <string.h>
#include <stdlib.h>

#ifdef ESP8266
#include <stdio.h>
#endif

/* We want to find if either
   (1) substr is contained in buffer at position 'startbuffer' OR if
   (2) the beginning of substr if contained at the end of buffer.
   * This function returns
   * 0 iff not (1) and not (2)
   * -1 if (1)
   * k>0 iff (2) and k is the first index of substr not contained in buffer.
   */
int firstdiff(const char * buffer, const char * substr, int startbuffer, int startindex) {
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

bool common::StringAwaiter::read(const char * buffer) {
  first_not_found=find(buffer,m_wanted,first_not_found);
  if (first_not_found==-1) {
    first_not_found=0;
    return true;
  }
  return false;
}

const char * common::StringAwaiter::wanted() const {
  return m_wanted;
}

int common::StringAwaiter::test() {
  using namespace common;
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
    StringAwaiter a("123456");
    assert(!a.read("aaa123"));
    assert(!a.read("aaa456"));
    assert(!a.read("aaa123"));
    assert(a.read("456bbb"));
  }
  
  {
    StringAwaiter a("1234567890");
    assert(!a.read("---12"));
    assert(!a.read("3456"));
    assert(a.read("7890ssss"));  
  }
  
  {
    DBG("*******\n");
    StringAwaiter a("OK");
    assert(a.read("OXOK"));
  }

  {
    DBG("*******\n");
    StringAwaiter a("ERROR");
    assert(!a.read("ERR"));
    assert(a.read("OR\n"));
  }

  {
    DBG("*******\n");
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
  DBG("GOOD\n");
  return 0;
}
