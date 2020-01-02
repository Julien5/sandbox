#include "application_esp8266.h"
#include "eeprom.h"
#include <stdio.h>
#include "debug.h"
#include "rtcmemory.h"
#include "sleep.h"

#define LEN (512-36)

// dumps raw memory in hex byte and printable split format
void dump(const unsigned char *data_buffer, const unsigned int length) {
  unsigned char byte;
  unsigned int i, j;
  for(i=0; i < length; i++) {
    byte = data_buffer[i];
    printf("%02x ", data_buffer[i]);  // display byte in hex
    if(((i%16)==15) || (i==length-1)) {
      for(j=0; j < 15-(i%16); j++)
	printf("   ");
      printf("| ");
      for(j=(i-(i%16)); j <= i; j++) {  // display printable bytes from line
	byte = data_buffer[j];
	if((byte > 31) && (byte < 127)) // outside printable char range
	  printf("%c", byte);
	else
	  printf(".");
      }
      printf("\n"); // end of the dump line (each line 16 bytes)
    } // end if
  } // end for
}


void maintask(void *pvParameters)
{
  printf("maintask\n");
  // printf("eeprom test: %d\n",eeprom::test());
  rtcmemory R;
  unsigned char data0[LEN]={0};
  if (!R.read(data0,sizeof(data0)))
    printf("could not read\n");
  dump(data0,LEN);
  char data1[LEN]={0};
  for(unsigned k=0;k<LEN;++k)
    data1[k]=0xFF;
  if (!R.write(data1,sizeof(data1)))
    printf("could not write\n");
  sleep S;
  S.deep_sleep(500);
  while (true) {};
}
