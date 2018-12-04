#pragma once

#include <string.h>

namespace parse {
  class StringAwaiter
  {
    const char * wanted; // 0-terminated
    int first_not_found;
  public:
    StringAwaiter(const char *w)
      :wanted(w)
      ,first_not_found(0)
    {};
    bool read(const char * buffer);
  };

  int test();
}
