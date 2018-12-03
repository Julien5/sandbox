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
    ref z;
    char * tok_addr;
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
    std::size_t capacity() const { return N; }
    const char* c_str() const { return s; }
    
    
    friend std::ostream& operator<<(std::ostream& os, STR s)
    {
      return os.write(s.c_str(), s.size());
    }

    void append(const char * b)
    {
      strcat(s,b);
    }

    bool contains(const char *b) {
      return !strcmp(s,b);
    }

    char * zeroes(const char *b) {
      tok_addr=s;
      char *a=::strtok(s,b);
      while(::strtok(NULL,b))
	{}
      return tok_addr;
    }

    char * tok() {
      return tok_addr;
    }

    char * zeroes() {
      for(int k=0;k<N;++k) {
	z[k] = (s[k]=='\0') ? '0' : '*';
      }
      z[N-1]='\0';
      return z;
    }

    char * next_tok() {
      if (tok_addr-s >= N) {
	tok_addr=0;
	return tok_addr;
      }
      tok_addr += strlen(tok_addr)+1;
      if (strlen(tok_addr)==0) {
	tok_addr=0;
      }
      return tok_addr;
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
