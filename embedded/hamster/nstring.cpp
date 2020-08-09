#include "nstring.h"
#include "debug.h"

int
nstring::test()
{
  auto x = nstring::make("hello");
  auto y = nstring::make(" foo ");
  auto z = x + y;

  auto a = nstring::make("aaa,bb\r\n+cc"); // 9 chars
  a.zeroes(",\r\n+");
  while (char* p = a.tok()) {
    assert(p);
  }
  return 0;
}
