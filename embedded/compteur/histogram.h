#pragma once

#include "common/debug.h"
#include <cassert>
#include <cstdlib>

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

  int compare(const void *cmp1, const void *cmp2) {
    Bin a = *(Bin*)(cmp1);
    Bin b = *(Bin*)(cmp2);
    return b.value - a.value;
  }
  
  void sort(Bin * begin, Bin * end) {
    const auto size_array=end-begin;
    const auto size_element=sizeof(*begin);
    qsort(begin, size_array, size_element, compare);      
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
  void update(uint16_t value) {
    for(size_t k=0;k<NBINS;++k) {
      // could be faster, since the container is sorted.
      if (bins[k].value == value || bins[k].count==0) {
	bins[k].value = value;
	bins[k].count++;
	impl::sort(bins,bins+NBINS);
	return;
      }
    }
    DBG("failed to insert %d\n",value);
    assert(0);
  }
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
  uint16_t min() const {
    Bin b;
    for_each_valid_bin([&](const Bin& bin) {
	b=bin;
      });
    return b.value;
  }
  uint16_t max() const {
    Bin b;
    for_each_valid_bin([&](const Bin& bin) {
	if (b.value==0)
	  b=bin;
      });
    return b.value;
  }
  size_t threshold(int percent) const {
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
  
  static int test() {
    DBG("testing\n");
    {
      Histogram H;
      assert(H.size()==0);
      H.update(2);
      H.update(2);
      H.update(3);
      H.update(1);
      assert(H.count()==4);
      assert(H.size()==3);
      assert(H.min()==1);
      assert(H.max()==3);
      assert(H.threshold(10)==3);
      assert(H.threshold(100)==1);
    }
    {
      Histogram H;
      for(int k=15;k!=0;--k)
	H.update(k%10);
      assert(H.count()==15);
      assert(H.size()==10);
      assert(H.min()==0);
      assert(H.max()==9);
      assert(H.threshold(50)==4);
    }
    DBG("good\n");
    return 0;
  }
};
