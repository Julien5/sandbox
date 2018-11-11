#include "parse.h"
#include "debug.h"

/*
  +IPD,116:HTTP/1.0 200 OK
  Server: BaseHTTP/0.6 Python/3.4.2
  Date: Sat, 10 Nov 2018 18:48:57 GMT
  Content-type: text/html


  +IPD,11:thanks,bye
  CLOSED
*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

bool parse::StringAwaiter::read(const char * buffer) {
  const char * f = buffer;
  while(strlen(f)>0) {
    f = strchr(f,notfound[0]);
    if (!f)
      return false;
    if (notfound != wanted && f != buffer)
      return false;
    int Lw=strlen(notfound);
    int Lf=strlen(f);
    debug(Lw);
    debug(Lf);
    debug(buffer);
    debug(f);
    debug(notfound);
    int L=MIN(Lw,Lf);
    int n=strncmp(f,notfound,L);
    debug(n);
    if (n==0) {
      debug(n);
      if (Lw>Lf) {
	notfound+=Lf;
	return false;
      }
      debug(n);
      notfound=wanted;
      return true;
    } else {
      f++;
    }
  }
  notfound=wanted;
  return false;
}

int parse::test() {
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
