#pragma once

#define NTICKS 16

class statistics {
public:
  using time = unsigned long;
  
private:
  struct Ticks {
    int delta[NTICKS];
    time t0;
    Ticks():
      delta{0}
      ,t0(0)
    {}
  };
  
  Ticks ticks;
  int index;
  int total_count;
   
  bool load();
  void increment_count(const time m, int incr);
  
public:
  statistics();
  void clear();
  void save();
  void increment_count(const time m);
  int get_count() const;
  int get_total_count() const;
  char *getdata(time m, int * Lout);
  int elapsed(const time m) const;
  static int test();
};
