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

  auto a = nstring::make("aaa,bb;cc"); // 9 chars
  debug(a);
  debug(a.capacity());
  debug(a.zeroes());
  a.zeroes(";,;");
  debug(a);
  debug(a.zeroes());
  while(char *p=a.tok()) {
    debug(p);
    assert(p);
    a.next_tok();
  }
  return 0;
}

