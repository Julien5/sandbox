#include "crc.h"
#include "common/debug.h"
// http://www.devcoons.com/crc8/
uint8_t crc::CRC8(const uint8_t *data, size_t length) 
{
  uint8_t crc = 0x00;
  uint8_t extract;
  uint8_t sum;
  for(size_t i=0; i<length; i++) {
    extract = *data;
    for (uint8_t tempI = 8; tempI; tempI--) {
	sum = (crc ^ extract) & 0x01;
	crc >>= 1;
	if (sum)
	  crc ^= 0x8C;
	extract >>= 1;
    }
    data++;
  }
  return crc;
}

int crc::test() {
  uint8_t data[16];
  for(size_t k=0; k<sizeof(data); ++k) {
    data[k]=k & 0xFF;
  }
  const auto orig = CRC8(data,sizeof(data));
  printf("orig: 0x%02x\n", orig);
  for(size_t j=0; j<sizeof(data); ++j) {
    for(size_t k=0; k<sizeof(data); ++k) {
      data[k]=k & 0xFF;
    }
    // inverse bit at position j modulo 8.
    const uint8_t mini= 1 << (j % 8);
    const auto a = data[j];
    data[j] ^= mini;
    const auto b = data[j];
    assert(a!=b);
    const auto c = CRC8(data,sizeof(data));
    DBG("j=%d, mini=0x%02x, a=0x%02x, b=0x%02x => crc: 0x%02x vs 0x%02x, OK=%d\n",j,mini,a,b,c,orig,int(c != orig));
  }
  return 0;
}
