#pragma once

struct Ticks {
  int delta[4];
  long long t0;
  Ticks():
    delta({-1})
    ,t0(-1)
  {}
};

class statistics {
public:
  using time = long long;
  
private:
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
