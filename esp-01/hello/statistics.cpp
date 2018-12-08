#include "statistics.h"

#define N 60

statistics::statistics()
  : time({0})
  , count({0})
  , index(0)
{}

void statistics::clear() {
  for(int k=0; k<N; ++k) {
    time[k]=0;
    count[k]=0;
  }
}

void statistics::increment_count(const int m) {
  int last_minute=time[index];
  int current_second = m/1000;
  int current_minute = current_second/60;
  if (current_minute != last_minute) {
    index++;
    time[index] = current_minute;
  }
  count[index]++;  
}
