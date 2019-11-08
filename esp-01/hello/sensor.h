#pragma once

#include "clock.h"
#include "defines.h"
#include "platform.h"

class sensor {
  Clock::ms last_time_rising_reed=0;
  atomic<bool> wake_on_rising_reed;
public:
  sensor():wake_on_rising_reed(false){}    
  void on_rising_reed() {
    wake_on_rising_reed.store(true);
  }    
  bool has_ticked();
};
