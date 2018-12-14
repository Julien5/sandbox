#include "statistics.h"
#include "debug.h"
#include "eeprom.h"

#define N 60

statistics::statistics()
  : time({0})
  , count({0})
  , index(-1)
  , first_millis(-1)
  , total_count(0)
{
  debug(first_millis);
  if (!load()) {
    debug("failed to load");
    clear();
    debug(first_millis);
  }
  debug(first_millis);
}

void statistics::clear() {
  for(int k=0; k<N; ++k) {
    time[k]=-1;
    count[k]=0;
  }
  save();
}

#define MAGIC 77
#define BUFFER_LENGTH 130

void statistics::save() {
  char d[BUFFER_LENGTH]={0};
  int k=0;
  d[k++]=MAGIC; // magic number
  d[k++]=(char)index;
  char * c = 0;
  c=(char *) &first_millis;
  for(int i=0; i<4; i++)
    d[k++]=c[i];
  
  c=(char *) &total_count;
  for(int i=0; i<4; i++)
    d[k++]=c[i];

  for(int i=0; i<N; i++)
    d[k++]=time[i];
  for(int i=0; i<N; i++)
    d[k++]=count[i];
  
  
  for(k=0;k<BUFFER_LENGTH;++k)
    eeprom().write(k,d[k]);
}

bool statistics::load() {
  char d[BUFFER_LENGTH]={0};
  for(int k=0;k<128;++k)
    d[k]=eeprom().read(k);
 
  int k=0;
  char magic_number=d[k++];
  if (magic_number != MAGIC) {
    return false;
  }
  
  index=int(d[k++]);
  
  char c[4]={0};  
  for(int i=0; i<4; i++)
    c[i]=d[k++];
  first_millis = *((unsigned long*)(&c));
  
  for(int i=0; i<4; i++)
    c[i]=d[k++];
  total_count = *((int*)(&c));
  
  for(int i=0; i<N; i++)
    time[i]=d[k++];
  for(int i=0; i<N; i++)
    count[i]=d[k++];
  return true;
}

void statistics::start(const unsigned long m, bool hard) {
  if (!hard && first_millis>0) {
    unsigned long delta = m - first_millis;
    if ((delta/1000) < 60) {
      // remember during 1 minute.
      debug("keep");
      return;
    }
  }
  clear();
  first_millis=m;
  index=0;
  time[index]=0;
  count[index]=0;
}

long statistics::elapsed(const unsigned long m) const {
  return m - first_millis;
}

void statistics::increment_count(const unsigned long m, int incr) {
  assert(first_millis>=0);  
  const unsigned long delta = m - first_millis;
  //ticks[index]=delta;
  unsigned long current_second = delta/1000;
  unsigned long current_minute = current_second;///60;

  unsigned long last_minute=time[index];
  if (current_minute != last_minute) {
    index++;
  }
  time[index] = current_minute;
  count[index] = count[index] + incr;
  total_count = total_count + incr;
}
void statistics::increment_count(const unsigned long m) {
  increment_count(m,1);
}

int statistics::get_count() const {
  int ret=0;
  for(int t=0; t<N; ++t) {
    if (time[t]<0)
      continue;
    ret+=count[t];
  }
  return ret;
}

int statistics::get_total_count() const {
  return total_count;
}

void statistics::getdata(unsigned long m, data &addr, int * Lout) {
  if (get_count()==0)
    return;
  
  increment_count(m,0);
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
  {
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

    assert(s.time[0]==0);
    assert(s.count[0]==0);
    assert(s.time[1]==2);
    assert(s.count[1]==10);
    assert(s.time[2]==10);
    assert(s.count[2]==10);
    assert(s.time[3]==40);
    assert(s.count[3]==10);
    s.save();
  }
  debug("ok");
  {
    statistics s;
    assert(s.time[0]==0);
    assert(s.count[0]==0);
    assert(s.time[1]==2);
    assert(s.count[1]==10);
    assert(s.time[2]==10);
    assert(s.count[2]==10);
    assert(s.time[3]==40);
    assert(s.count[3]==10);
  }
  debug("good");
  return 0;
}
