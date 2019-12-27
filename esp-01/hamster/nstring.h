#pragma once

#include <string.h>
#include <stdlib.h>
#include "debug.h"

namespace nstring {
  template<int N>
  class STR
  {
  private:
    using ref = char [N];
    ref s;
    char z[N+1];
    char * tok_addr = 0;
  public:
    STR(const char* x)
      : s{0},z{0}
    {
      strcat(s,x);
    }
    
    STR()
      : s{0},z{0}
    {}
    
    int size() const { return strlen(s); }
    bool empty() const {
      return s[0]=='\0';
    }
    void clear() {
      memset(s,'\0',N);
    }
    int capacity() const { return N; }
    const char* c_str() const { return s; }
    
    
#ifndef ARDUINO
    friend std::ostream& operator<<(std::ostream& os, STR s)
    {
      return os.write(s.c_str(), s.size());
    }
#endif

    void append(const char * b)
    {
      strcat(s,b);
    }

    bool contains(const char *b) {
      return strstr(s,b)!=0;
    }

    bool contains(const char b) {
      return strchr(s,b) != 0;
    }

    bool operator==(const char *b) {
      return strcmp(s,b)==0;
    }
    bool operator==(const STR<N> &b) {
      return operator=(b.c_str());
    }
    
    void replace(char a, char b) {
      for(int k=0; k<N; ++k) {
	if (s[k]==a)
	  s[k]=b;
      }
    }

    int toInt() {
      return atoi(s);
    }
    
    char * zeroes(const char *b) {
      tok_addr=0;
      const int Nb=strlen(b);
      for(int k=0; k<N; ++k) {
	for(int n=0;n<Nb; ++n) {
	  if (s[k]==b[n]) {
	    s[k] = '\0';
	    break;
	  }
	}
      }
      return tok_addr;
    }

    char * zeroes() {
      for(int k=0;k<N;++k) {
	z[k] = s[k];
	if (s[k] == '\0')
	  z[k] = '_';
	if (s[k] == '\r')
	  z[k] = '#';
	if (s[k] == '\n')
	  z[k] = '#';
      }
      z[N]='\0';
      return z;
    }

    char * tok() {
      if (tok_addr==0) {
	tok_addr=s;
	return tok_addr;
      }
      tok_addr += strlen(tok_addr);
      while(*tok_addr == '\0')
	tok_addr++;
      if (tok_addr-s >= N || strlen(tok_addr)==0) {
	tok_addr=0;
      }
      return tok_addr;
    }
    
  };

  template<int N, int M>
  STR<N+M> operator+(const STR<N> &a, const STR<M> &b) {
    STR<N+M> r;
    r.append(a.c_str());
    r.append(b.c_str());
    return r;
  }

  template<int N>
  STR<N> make(const char (&s)[N]) {
    return STR<N>(s);
  }

  int test();
  
}
