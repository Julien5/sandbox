#pragma once

#include <string.h>

namespace parse {
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
  };

  class MessageParser {
    StringAwaiter startAwaiter;
    StringAwaiter endAwaiter;
    char message[32]; 
    char state;
  public:
    MessageParser()
      : startAwaiter("{")
      , endAwaiter("}")
      , message{0}
      , state{0} // 0 = start, 1 = { found, 2 = } found
    {}
    void read(char * buffer);
    char* get() {
      if (state != 2)
	return 0;
      return &(message[0]);
    }
    void reset();
  };

  int test();
}
