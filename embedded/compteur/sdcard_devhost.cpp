#include "sdcard.h"
#include "debug.h"
#include <iostream>
#include <fstream>

sdcard::sdcard() {}

void sdcard::init() {
}

void sdcard::info() {
}

#include "time.h"

void sdcard::write(const char * filename, const uint8_t * data, const size_t length) {
  std::ofstream f("/tmp/sdcard/"+std::string(filename), std::ios::out | std::ios::binary);
  if(!f) {
    DBG("could not open %s\n",filename);
    return;
  }
  f.write((char*)data,length);
  f.close();  
}

