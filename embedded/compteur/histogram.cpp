#include "histogram.h"
#include <stdlib.h>

namespace impl {
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

void Histogram::update(uint16_t value) {
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

int Histogram::test() {
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
    assert(H.minimum()==1);
    assert(H.maximum()==3);
    assert(H.threshold(10)==3);
    assert(H.threshold(100)==1);
  }
  {
    Histogram H;
    for(int k=15;k!=0;--k)
      H.update(k%10);
    assert(H.count()==15);
    assert(H.size()==10);
    assert(H.minimum()==0);
    assert(H.maximum()==9);
    assert(H.threshold(50)==4);
  }
  DBG("good\n");
  return 0;
}
