#pragma once

class statistics {
  void clear();
  int time[60];
  int count[60];
  int index;
public:
  statistics();
  void increment_count(const int m);
};
