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

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

bool parse::StringAwaiter::read(const char * buffer) {
  std::cout << std::endl;
  int k=0;
  std::cout << " w:" << wanted << std::endl;
  std::cout << " b:" << buffer << std::endl;
  std::cout << "nf:" << notfound << std::endl;
  {
    std::cout << "----" << std::endl;
    const char * f = strchr(buffer,notfound[0]);
    if (!f)
      return false;
    int Lw=strlen(notfound);
    int Lf=strlen(f);
    int L=MIN(Lw,Lf);
    std::cout << "f:" << f << std::endl;
    std::cout << "L:" << L << std::endl;
    int n=strncmp(f,notfound,L);
    std::cout << "n=" << n << std::endl;
    if (n==0) {
      if (Lw>Lf) {
	notfound+=Lf;
	return false;
      }
      notfound=wanted;
      return true;
    }
    notfound=wanted;
    buffer=f;
  }	
  return false;
}

int parse::test() {
  {
    StringAwaiter a("OK");
    bool ok=false;
    if (a.read("OK")==false)
      return 1;
    if (a.read("aaaOKaaa")==false)
      return 1;
    ok=a.read("buuuh");
    ok=a.read("buuuO");
    if (ok)
      return 1;
    ok=a.read("KO");
    if (!ok)
      return 1;
    ok=a.read("buuuO");
    ok=a.read("xKO");
    if (ok)
      return 1;
    return 0;
  }
}
