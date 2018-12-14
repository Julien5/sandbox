#pragma once

class statistics {
  void clear();
    //char data[256];
  char time[60];
  char count[60];

  int index;
  long first_millis;
  int total_count;
  
  bool load();
  void increment_count(const unsigned long m, int incr);
  
public:
  statistics();
  void save();
  void start(const unsigned long m, bool hard=false);
  void increment_count(const unsigned long m);
  int get_count() const;
  int get_total_count() const;

  using data = char[128];
  void getdata(unsigned long m, data &addr, int * Lout);

  long elapsed(const unsigned long m) const;
  
  static int test();
};
