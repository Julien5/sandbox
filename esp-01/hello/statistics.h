#pragma once

namespace types
{
  using minute = unsigned int;
  using count = unsigned char;
  using milli = unsigned int;
}
constexpr int NMILLIS=2;
constexpr int NMINUTES=2;
constexpr int NDATA=(sizeof(types::minute)+sizeof(types::count))*NMINUTES
  + sizeof(types::milli)*NMILLIS + 1;

class statistics {
private:
  char data[NDATA];
  void clear();

public:
  statistics();
  void tick();
  void save();
  bool load();
  int total();
  int minute_count();
  void get_minute(const int m, types::minute *, types::count *);
  char *getdata(int * Lout);
  bool operator==(const statistics& other);
  static int test();
};
