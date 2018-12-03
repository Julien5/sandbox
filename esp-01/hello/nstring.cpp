#include "nstring.h"
#include "debug.h"

/*
auto nstring::make(const char *s) {
  return STR<16>(s);
}
*/


int nstring::test() {
  auto x = nstring::make("hello");
  debug(x);
  auto y = nstring::make(" foo ");
  auto z = x + y; 
  debug(z);

  auto a = nstring::make("aaa,bb\r\n+cc"); // 9 chars
  debug(a);
  a.zeroes(",\r\n+");
  debug(a);
  debug(a.zeroes());
  while(char *p=a.tok()) {
    debug(p);
    assert(p);
  }
  return 0;
}

