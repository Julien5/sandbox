#pragma once

#define NMILLIS 60
#define NMINUTES 200
#define NDATA 1024
class statistics {
private:
  char data[NDATA];
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
