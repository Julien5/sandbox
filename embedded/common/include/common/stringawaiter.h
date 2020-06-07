#pragma once

namespace common {
  class StringAwaiter
  {
    const char * m_wanted; // 0-terminated
    int first_not_found;
    bool m_found;
  public:
    StringAwaiter(const char *w)
      :m_wanted(w)
      ,first_not_found(0)
    {};
    bool read(const char * buffer);
    const char * wanted() const;
    static int test();
  };
}
