#include "nstring.h"
#include "debug.h"

auto nstring::make(const char *s) {
  return STR<16>(s);
}


int nstring::test() {
  auto x = nstring::make("hello");
  debug(x);
  auto y = nstring::make(" foo ");
  x = x + y; 
  x.append("bar");
  debug(x);  
  return 0;
}

