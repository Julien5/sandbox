#pragma once

class statistics {
  void clear();
  int time[60];
  int count[60];
  int index;
  unsigned long first_millis;
public:
  statistics();
  void start(const unsigned long m);
  void increment_count(const unsigned long m);

  using data = char[128];
  void getdata(data &addr, int * Lout);

  static int test();
};
