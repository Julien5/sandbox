#include "statistics.h"
#include "debug.h"
#include "eeprom.h"

#define N 4

statistics::statistics()
  : index(0)
  , total_count(0)
{
  if (!load()) {
    debug("failed to load");
    clear();
  }
}

void statistics::clear() {
  for(int k=0; k<N; ++k) {
    ticks.delta[k]=-1;
  }
  ticks.t0=-1;
  index=0;
  save();
}

#define MAGIC 78

void write_to_eeprom(char *c, unsigned char length, int *index) {
  int k=*index;
  for(int i=0; i<length; i++)
    eeprom().write(k++,c[i]);
  *index=k;
}

// TODO: template
void write_int_to_eeprom(int input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(int),index);
}

void write_longlong_to_eeprom(long long input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(long long),index);
}

void statistics::save() {
  int k=0;
  eeprom().write(k++,MAGIC);  
  write_int_to_eeprom(index,&k);
  write_int_to_eeprom(total_count,&k);
  for(int i=0; i<N; i++)
    write_int_to_eeprom(ticks.delta[i],&k);
  write_longlong_to_eeprom(ticks.t0,&k);
}

void read_from_eeprom(char *c, unsigned char length, int *index) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++)
    c[i]=eeprom().read(k++);
  *index=k;
}

void read_int_from_eeprom(int *result, int *index) {
  char c[4]={0};
  read_from_eeprom(&c[0],4,index);
  *result=*((int*)(&c));
}

void read_longlong_from_eeprom(long long *result, int *index) {
  char c[8]={0};
  read_from_eeprom(&c[0],8,index);
  *result=*((long long*)(&c));
}

bool statistics::load() {
  int k=0;
  char magic_number=eeprom().read(k++);
  if (magic_number != MAGIC) {
    return false;
  }
  read_int_from_eeprom(&index,&k);
  read_int_from_eeprom(&total_count,&k);
  for(int i=0; i<N; i++) {
    read_int_from_eeprom(&(ticks.delta[i]),&k);
  }
  read_longlong_from_eeprom(&(ticks.t0),&k);
  return true;
}

int statistics::elapsed(const time m) const {
  return int(m - ticks.t0);
}

void statistics::increment_count(const time m, int incr) {
  if (ticks.t0<0) {
    ticks.t0=m;
  }
  assert(ticks.t0!=-1);
  const unsigned int delta = m - ticks.t0;
  assert(index<sizeof(ticks.delta)/sizeof(ticks.delta[0]));
  ticks.delta[index++]=delta;
  total_count = total_count + incr;
}
void statistics::increment_count(const time m) {
  increment_count(m,1);
}

int statistics::get_count() const {
  int ret=0;
  for(int t=0; t<N; ++t) {
    if (ticks.delta[t]!=-1)
      ret++;
  }
  return ret;
}

int statistics::get_total_count() const {
  return total_count;
}

char * statistics::getdata(time m, int * Lout) {
  if (get_count()==0)
    return 0;
  const int t0=ticks.t0;
  assert(t0>0);
  assert(ticks.t0==t0);
  *Lout = sizeof(ticks)/1; // in bytes
  return (char*)&ticks;
}

int statistics::test() {
  const int sec=1000;
  const int min=60*sec;
  time t0=2*min;
  {
    statistics s;
    s.clear();
    for(int k=0; k<N; ++k)
      s.increment_count(t0+k);
    for(int k=0; k<N; ++k) {
      assert(s.ticks.delta[k]==k);
    }
    assert(s.ticks.t0==t0);
    s.save();
  }
  debug("ok");
  {
    statistics s;
    for(int k=0; k<N; ++k){
      assert(s.ticks.delta[k]==k);
    }
    assert(s.ticks.t0==t0);
    int L=0;
    char * d = s.getdata(t0+60,&L);
    assert(s.ticks.t0==t0);
  }
  debug("good");
  return 0;
}
