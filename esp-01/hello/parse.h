#pragma once

#include <string.h>

namespace parse {
  class StringAwaiter
  {
    const char * wanted; // 0-terminated
    const char * notfound;
  public:
    StringAwaiter(const char *w)
      :wanted(w)
      ,notfound(w)
    {};
    bool read(const char * buffer);
  };

  int test();
}
