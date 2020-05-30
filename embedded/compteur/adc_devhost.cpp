#include "adc.h"
#include <cassert>
#include "common/debug.h"

#undef FILE

#ifdef FILE

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace file {
  std::string read_file(const std::string &fileName)
  {
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::ate);
    std::ifstream::pos_type s = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(s);
    ifs.read(bytes.data(), s);
    return std::string(bytes.data(), s);
  }

  std::vector<std::string> split(const std::string &s, const std::string &sep) {
    size_t beg=0;
    size_t end=0;
    std::vector<std::string> ret;
    while(true) {
      end=s.find(sep,beg);
      if (end==std::string::npos)
	break;
      auto part=s.substr(beg,end-beg);
      ret.push_back(part);
      beg=end+1;
      end=0;    
    } 
    return ret;
  }

  std::vector<std::string> lines(const std::string &s) {
    auto ret=split(s,"\n");
    return ret;
  }

  std::vector<uint16_t> numbers(const std::vector<std::string> &lines) {
    std::vector<uint16_t> ret;
    for(auto & line : lines) {
      const auto P=split(line,"\t");
      ret.push_back(std::stoi(P[1]));
    }
    return ret;
  }

  std::vector<uint16_t> s_numbers;
  size_t counter=0;
  void get_data() {
    std::string s=read_file("cut.csv");
    s_numbers=numbers(lines(s));
  }

  uint16_t generate() {
    using namespace file;
    if (s_numbers.empty()) {
      get_data();
    }
    assert(!s_numbers.empty());
    return s_numbers[(counter++) % s_numbers.size()];
  }
}
using namespace file;

#else

#include "common/clock.h"
#include <cstdlib>
namespace synthetic {
  int8_t inoise(double x)
  {
    const auto b1=double(1)/3;
    const auto b2=2*b1;
    if (x<b1)
      return -1;
    if (x<b2)
      return 0;
    return 1;
  }
  uint16_t noise() {
    const double n=double(std::rand())/RAND_MAX; // [0,1]
    return inoise(n);
  }
  constexpr uint16_t c = 4000;
  constexpr uint32_t T = 1000*3.6*1e6/(75*c);
  constexpr uint32_t T1 = 5*T/100;
  constexpr uint16_t m = 33;
  constexpr uint16_t M = 37;
  uint16_t generate() {
    auto t=Clock::millis_since_start();
    assert(T1>=1);
    if (t%T<=T1)
      return M+noise();
    return m+noise();
  }
}
using namespace synthetic;

#endif

uint16_t analog::read() {
  return generate();
}
