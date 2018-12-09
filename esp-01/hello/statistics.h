#pragma once

class statistics {
  void clear();
  char time[60];
  char count[60];
  char index;
  long first_millis;

  bool load();
  
public:
  statistics();
  void save();
  void start(const unsigned long m);
  void increment_count(const unsigned long m);

  using data = char[128];
  void getdata(data &addr, int * Lout);

  static int test();
};
