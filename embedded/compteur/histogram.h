#pragma once

#include "common/debug.h"

struct Bin {
  uint16_t value=0;
  uint32_t count=0;
};

constexpr size_t NBINS=10;

namespace impl {
  // no stl in arduino.
  template<typename I, typename P>
  void for_each(I b, I e, P p) {
    for(I it=b; it!=e; ++it)
      p(*it);
  }
}

class Histogram {
  Bin bins[NBINS];
  template<typename F>
  void for_each_valid_bin(F f) const {
    impl::for_each(bins,bins+NBINS,[&](const Bin& bin) {
	if (bin.count!=0)
	  f(bin);
      });
  }
public:
  void print() {
    for(size_t k=0;k<NBINS;++k) {
      DBG("k=%d count=%d value=%d\n",k,bins[k].count,bins[k].value);
    }
  }
  void update(uint16_t value);
  uint16_t size() const {
    uint16_t ret=0;
    for_each_valid_bin([&](const Bin& bin) {
	ret++;
      });
    return ret;
  }
  uint16_t count() const {
    uint16_t ret=0;
    for_each_valid_bin([&](const Bin& bin) {
	ret+=bin.count;
      });
    return ret;
  }
  uint16_t minimum() const {
    Bin b;
    for_each_valid_bin([&](const Bin& bin) {
	b=bin;
      });
    return b.value;
  }
  uint16_t maximum() const {
    Bin b;
    for_each_valid_bin([&](const Bin& bin) {
	if (b.value==0)
	  b=bin;
      });
    return b.value;
  }
  uint16_t threshold(int percent) const {
    const size_t wanted_count = count()*percent/100;
    size_t accumulated_count=0;
    Bin b=bins[0];
    for_each_valid_bin([&](const Bin& bin) {
	if (accumulated_count<wanted_count) {
	  accumulated_count+=bin.count;
	  b=bin;
	}
      });
    return b.value;
  }
  uint16_t least() {
    Bin b=bins[0];
    for_each_valid_bin([&](const Bin& bin) {
	if (bin.count<b.count)
	  b=bin;
      });
    return b.value;
  }  
  static int test();
};
