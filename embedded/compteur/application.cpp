#include "application.h"
#include "debug.h"
#include "time.h"
#include "sdcard.h"
#include "stdint.h"

#include <string.h>
#ifdef ARDUINO
#include "Arduino.h"
uint16_t analogRead() {
  return analogRead(0);
}
#endif

#ifdef ESP8266
#include "driver/adc.h"
#include "esp_sleep.h"
uint16_t analogRead() {
  uint16_t ret=0;
  // adc_read_fast(&ret,1);
  adc_read(&ret);
  return ret;
}
#endif

#ifdef DEVHOST
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
namespace devhost {
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
    std::string s=read_file("sd-data/23/hex/data.csv");
    s_numbers=numbers(lines(s));
  }
}

uint16_t analogRead() {
  using namespace devhost;
  if (s_numbers.empty()) {
    get_data();
  }
  assert(!s_numbers.empty());
  return s_numbers[(counter++) % s_numbers.size()];
  
}
#endif

 
sdcard sd;

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("@START");
#endif

#ifdef ESP8266
  // esp_wifi_fpm_set_sleep_type(WIFI_NONE_SLEEP_T);
  adc_config_t config;
  config.mode=ADC_READ_TOUT_MODE;
  config.clk_div=8;
  auto err=adc_init(&config);
  DBG("err=%d\r\n",err);
#endif
  
  sd.init();
  sd.info();
  const char * d = "ffff.ggg";
  sd.write("foo.txt",(uint8_t*)d,strlen(d));
}

uint16_t data[256] = {0};
uint16_t indx=0;
uint16_t counter=0;


class histogram {
  static constexpr size_t N=16;
  uint16_t occurences[N];
  uint8_t values[N];
public:
  int8_t index(const uint8_t &v) {
    for(int k=0;k<N;++k) 
      if (values[k]==v)
	return k;
    return -1;
  }
  void reg(const uint8_t &v) {
    auto i=index(v);
    if (i<0) {
      for(int k=0;k<N;++k) 
	if (occurences[k]==0) {
	  i=k;
	  break;
	}
    }
    values[i]=v;
    occurences[i]++;
  }
  void print() {
    for(int k=0;k<N;++k) {
      DBG("value: %d #=%d\r\n",values[k],occurences[k]);
    }
  }
  
};

histogram h;

void application::loop()
{ 
  int a=analogRead();
  h.reg(a);
  data[indx++]=a;
  DBG("data[%d]=%d\r\n",indx,a);
  if (indx>=sizeof(data)/sizeof(uint16_t)) {
    char filename[13]; // 8.3 => 8+1+3+1 (zero termination) => 13 bytes.
    sprintf(filename,"%08u.BIN",counter++);
    DBG("writing %s\r\n",filename);
    sd.write(filename,(uint8_t*)data,sizeof(data));
    indx=0;
    DBG("mem:%d\r\n",debug::freeMemory());
    h.print();
    // exit(0);
  }
  time::delay(10);
}
