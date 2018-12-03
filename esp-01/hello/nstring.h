#pragma once

#include <cstddef>
#include <ostream>
#include <string.h>

#include <iostream>
#include <cassert>
#include "debug.h"


namespace nstring {
  template<int N>
  class STR
  {
  private:
    using ref = char [N];
    ref s;
  public:
    STR(const char* x)
      : s{}
    {
      strcat(s,x);
    }
    
    STR()
      : s{}
    {}
    
    std::size_t size() const { return strlen(s); }
    const char* c_str() const { return s; }


    void append(const char * b)
    {
      strcat(s,b);
    }
    
    friend std::ostream& operator<<(std::ostream& os, STR s)
    {
      return os.write(s.c_str(), s.size());
    }

    

    
  };

  template<int N, int M>
  auto operator+(const STR<N> &a, const STR<M> &b) {
    STR<N+M> r;
    r.append(a.c_str());
    r.append(b.c_str());
    return r;
  }

  template<int N>
  auto make(const char (&s)[N]) {
    return STR<N>(s);
  }

  int test();
  
}
