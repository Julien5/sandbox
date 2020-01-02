#include "nstring.h"
#include "debug.h"

int nstring::test() {
  auto x = nstring::make("hello");
  DBG(x);
  auto y = nstring::make(" foo ");
  auto z = x + y; 
  DBG(z);

  auto a = nstring::make("aaa,bb\r\n+cc"); // 9 chars
  DBG(a);
  a.zeroes(",\r\n+");
  DBG(a);
  DBG(a.zeroes());
  while(char *p=a.tok()) {
    DBG(p);
    assert(p);
  }
  return 0;
}

