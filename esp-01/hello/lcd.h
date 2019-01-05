#pragma once

namespace display {
  class LCD {
  public:
    LCD();
    void init();
    void print(const char * msg);
  };
  
  static LCD lcd; 
}
