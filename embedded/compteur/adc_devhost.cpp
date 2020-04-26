#include "adc.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

namespace impl {
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
}

uint16_t analog::read() {
  using namespace impl;
  if (s_numbers.empty()) {
    get_data();
  }
  assert(!s_numbers.empty());
  return s_numbers[(counter++) % s_numbers.size()];
}
