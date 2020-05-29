#include "histogram.h"
#include <stdlib.h>
#include <cstring>

namespace impl {
  using namespace histogram;
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
  
  // no stl in arduino.
  template<typename I, typename F>
  void for_each(I b, I e, F f) {
    for(I it=b; it!=e; ++it)
      f(*it);
  }
}


histogram::packed::packed() {
  memset(bins,0,sizeof(bins));
}

histogram::Bin * histogram::Histogram::begin() const {
  return const_cast<histogram::Bin*>(m_packed.bins);
}

histogram::Bin * histogram::Histogram::end() const {
  Bin * it = begin();
  while(it->count != 0 && it<(m_packed.bins+NBINS))
    ++it;
  assert(it>=begin());
  return it;
}

uint16_t histogram::Histogram::size() const {
  return end()-begin();
}

uint16_t histogram::Histogram::least() const {
  if (size()==0)
    return 0;
  /* find a 'hole */
  for(auto it=begin(); (it+1) != end(); ++it) {
    const Bin &bin(*it);
    const Bin &next(*(it+1));
    if ((bin.value - next.value)>1) {
      Bin b;
      b.count = 0;
      b.value = (bin.value + next.value)/2;
      return b.value;
    }
  }
  
  Bin b=m_packed.bins[0];
  for(auto it = begin(); it != end(); ++it) {
    const Bin &bin(*it);
    if (bin.count<b.count)
      b=bin;
  }
  return b.value;
}  

uint16_t histogram::Histogram::count() const {
  uint16_t ret=0;
  impl::for_each(begin(),end(),[&](const Bin& bin) {
      ret+=bin.count;
    });
  return ret;
}

uint16_t histogram::Histogram::minimum() const {
  Bin b;
  impl::for_each(begin(),end(),[&](const Bin& bin) {
      b=bin;
    });
  return b.value;
}

uint16_t histogram::Histogram::maximum() const {
  Bin b;
  impl::for_each(begin(),end(),[&](const Bin& bin) {
      if (b.value==0)
	b=bin;
    });
  return b.value;
}

uint16_t histogram::Histogram::threshold(int percent) const {
  const size_t wanted_count = count()*percent/100;
  size_t accumulated_count=0;
  Bin b=m_packed.bins[0];
  impl::for_each(begin(),end(),[&](const Bin& bin) {
      if (accumulated_count<wanted_count) {
	accumulated_count+=bin.count;
	b=bin;
      }
    });
  return b.value;
}

void histogram::Histogram::update(uint16_t value) {
  for(size_t k=0;k<NBINS;++k) {
    // could be faster, since the container is sorted.
    if (m_packed.bins[k].value == value || m_packed.bins[k].count==0) {
      m_packed.bins[k].value = value;
      m_packed.bins[k].count++;
      impl::sort(m_packed.bins,m_packed.bins+NBINS);
      return;
    }
  }
  DBG("failed to insert %d\n",value);
  assert(0);
}

#ifdef DEVHOST
#include <fstream>
#endif

int histogram::Histogram::test() {
  DBG("testing\n");
  {
    Histogram H;
    DBG("size:%d\n",H.size());
    assert(H.size()==0);
    H.update(2);
    H.update(2);
    H.update(3);
    H.update(1);
    DBG("count:%d\n",H.count());
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
#ifdef DEVHOST
    size_t L=0;
    const packed * data = H.get_packed(&L);
    using namespace std;
    std::ofstream file;
    file.open("histogram.packed.bin", ios::out | ios::binary);
    file.write((char*)data,L);
#endif
  }
  DBG("ALL GOOD\n");
  return 0;
}
