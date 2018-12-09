#include "statistics.h"
#include "debug.h"

#define N 60

statistics::statistics()
  : time({0})
  , count({0})
  , index(-1)
  , first_millis(-1)
{
  clear();
}

void statistics::clear() {
  for(int k=0; k<N; ++k) {
    time[k]=-1;
    count[k]=0;
  }
}

void statistics::start(const unsigned long m) {
  first_millis=m;
  index=0;
  time[index]=0;
  count[index]=0;
}
void statistics::increment_count(const unsigned long m) {
  assert(first_millis>=0);  
  const unsigned long delta = m - first_millis;
  unsigned long current_second = delta/1000;
  unsigned long current_minute = 6*current_second/60;

  unsigned long last_minute=time[index];
  if (current_minute != last_minute) {
    index++;
  }
  time[index] = current_minute;
  count[index]++;  
}

void statistics::getdata(data &addr, int * Lout) {
  int k=0;
  for(int t=0; t<N; ++t) {
    if (time[t]<0)
      continue;
    addr[k++]=time[t];
    addr[k++]=count[t];
  }
  *Lout = k;
}

int statistics::test() {
  const int sec=1000;
  const int min=60*sec;
 
  statistics s;
  s.clear();
  s.start(0);

  int t0=2*min;
  for(int k=0; k<10; ++k)
    s.increment_count(t0+k*sec);

  t0=10*min;
  for(int k=0; k<10; ++k)
    s.increment_count(t0+k*sec);

  t0=40*min;
  for(int k=0; k<10; ++k)
    s.increment_count(t0+k*sec);
  
  for(int t=0; t<N; ++t) {
    if (s.time[t]<0)
      continue;
    debug(t);
    debug(s.time[t]);
    debug(s.count[t]);
  }
}
