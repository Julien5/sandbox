#pragma once

namespace display {
  class LCD {
  public:
    LCD();
    void init();
    //void print(char * msg);
    void print(char * msg1, char * msg2=0);
    //void print(const char * msg);
    //void print(const char * msg1, const char *msg2);
  };
  
  static LCD lcd; 
}
