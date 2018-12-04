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

const int firstdiff(const char * buffer, const char * substr, int s0, int l0) {
  assert(s0<strlen(buffer));
  assert(l0<strlen(substr));
  int l = 0;
  while(buffer[s0+l] != 0 && substr[l0+l] != 0 && buffer[s0+l]==substr[l0+l]) {
    l++;
  }
  if (substr[l0+l]==0)
    return -1;
  if (buffer[s0+l]==0)
    return l0+l;
  return 0;
}

/* 
   -1 = found
   0  = not found 
   >0 = part found (up to k)
 */
int find(const char * buffer, const char * substr, int startindex=0) {
  const int Ls = strlen(substr);
  const int Lb = strlen(buffer);
  const bool must_be_first = startindex > 0;
  int k2=startindex;
  // invariant: k is the index of the first byte in substr not found in buffer.
  debug(buffer);
  debug(substr);
  int p=0;
  int k=0;
  while(p<Lb && (k=firstdiff(buffer, substr, p++, startindex))==0)
    debug(k);
  debug(p);
  k=firstdiff(buffer, substr, p-1, startindex);
  debug(k);
  return k;

  
  int l=0;
  bool eq=false;
  while(k==0 || (eq=buffer[l]==substr[k])) {
    debug(k);
    debug(l);
    l++; k++;
    if (k==Ls)
      return -1;
    if (l==Lb) // out of bound
      break;
    if (!eq)
      k=0;
  }
  debug(k);
  return k;

  for(int l=0; l<Lb; ++l) {
    debug("--");
    debug(l);
    debug(k);
    assert(k<Ls);
    debug(buffer[l] == substr[k]);
    debug(buffer[l]);
    debug(substr[k]);
    if (buffer[l] == substr[k])
      k++;
    else {
      k=0;
      if (buffer[l] == substr[k])
	k++;
      if (must_be_first) // none found
	break;
    }    
    if (k==Ls) // all found
      return -1;
  }
  assert(k<Ls);
  return k;
}

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
    int L=MIN(Lw,Lf);
    int n=strncmp(f,notfound,L);
    if (n==0) {
      if (Lw>Lf) {
	notfound+=Lf;
	return false;
      }
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
