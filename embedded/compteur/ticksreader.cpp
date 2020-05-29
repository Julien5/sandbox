#include "ticksupdater.h"
#include "adc.h"
#include "common/debug.h"
#include <math.h>

bool TicksReader::take() {
  const auto a = analog::read();
  DBG("a=%d\n",a);
  H.update(a);
  H.print();
  const auto TH = H.least();
  const auto TL = H.least()-1;
  assert(TL<=TH);

  DBG("a=%d TL=%d TH=%d threshold(20)=%d\n",a,TL,TH,H.threshold(20));
  
  if (H.least()>=H.threshold(20)) {
    DBG("not ready yet\n");
    return false;
  }
  /* check if histogram is ready */
  if ((H.maximum() - TH)<1) {
    DBG("not ready yet\n");
    return false;
  }
  if ((TL - H.minimum())<1) {
    DBG("not ready yet\n");
    return false;
  }
  /* TH should be close to the middle */
  if (fabs(TH-(H.maximum()+H.minimum())/2)>2) {
    DBG("not ready yet\n");
    return false;
  }
 
  /* is the value classificable ? */
  if (TL <= a && a <= TH) {
    DBG("no classificable %d\n",a);
    return false;
  }
  const auto new_value = a>TH;
  DBG("classificable %d\n",a);  
  if (new_value==m_last_value)
    return false;
  m_last_value=new_value;
  if (new_value==1) {
    return true;
  }
  return false;
}
