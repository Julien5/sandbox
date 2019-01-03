#pragma once

#define NMILLIS 16
#define NMINUTES 200

class statistics {
public:
  using minute = unsigned int;
  using count = unsigned char;
  using milli = unsigned int;

private:
  struct Bin {
    minute m; 
    count c;
  };
  Bin bins[NMINUTES];

  struct Milli {
    milli m;
  };
  Milli millis[NMILLIS];
  void clear();

public:
  statistics();
  void tick();
  char *getdata(int * Lout);
  static int test();
};
