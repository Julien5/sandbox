#include "parse.h"
#include <iostream>
/*
  +IPD,116:HTTP/1.0 200 OK
  Server: BaseHTTP/0.6 Python/3.4.2
  Date: Sat, 10 Nov 2018 18:48:57 GMT
  Content-type: text/html


  +IPD,11:thanks,bye
  CLOSED
*/
#include <cassert>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

bool parse::StringAwaiter::read(const char * buffer) {
    const char * f = strchr(buffer,notfound[0]);
    if (!f)
      return false;
    if (notfound != wanted && f != buffer)
      return false;
    int Lw=strlen(notfound);
    int Lf=strlen(f);
    int L=MIN(Lw,Lf);
    int n=strncmp(f,notfound,L);
    if (n==0) {
      if (Lw>Lf) {
	notfound+=Lf;
	return false;
      }
      notfound=wanted;
      return true;
    }
    notfound=wanted;
    return false;
}

int parse::test() {
  {
    StringAwaiter a("OK");
    bool ok=false;
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
  
  return 0;
}
