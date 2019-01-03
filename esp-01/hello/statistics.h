#pragma once

#define NMILLIS 2 // 60
#define NMINUTES 2 // 200

class statistics {
public:
  using minute = unsigned int;
  using count = unsigned char;
  using milli = unsigned int;

private:
  struct Data {
    struct Bin {
      minute m; 
      count c;
    };
    Bin bins[NMINUTES];
    
    struct Milli {
      milli m[NMILLIS];
      int index;
    };
    Milli millis;
  };
  Data data;
  void clear();

public:
  statistics();
  void tick();
  void save();
  bool load();
  char *getdata(int * Lout);
  bool operator==(const statistics& other);
  static int test();
};
