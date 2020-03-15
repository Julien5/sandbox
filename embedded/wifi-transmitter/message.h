#pragma once

#include <vector>
#include <stdint.h>
#include "platform.h"

namespace received {
  struct message {
    uint8_t data[2048];
    uint16_t length={0};
    message();
  };


  struct wifi_command {
    char command = 0;
    char * url = {0};
    uint16_t Ldata = 0;
    uint8_t * data = {0};
  };
  
  wifi_command read_wifi_command(const message &m);
   
  void test();
};
