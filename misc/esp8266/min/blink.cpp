/* The classic "blink" example
 *
 * This sample code is in the public domain.
 */
#include <stdlib.h>
#include "espressif/esp_common.h"
#include <espressif/spi_flash.h>
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include <string.h>
#include "foo.h"


#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysparam.h>

#include <espressif/spi_flash.h>
extern "C" {
#include <spiflash.h>
}
#include "espressif/esp_common.h"
#include "esp/uart.h"

void bang()
{
  printf("Hello world!\n");
  auto A = sdk_flashchip;
  // 2097152 = 2048K
  printf("chip size: %d\n",A.chip_size);

  //const uint32_t base_addr=0x3FA000;
  const uint32_t base_addr=0xF8000;
  uint32_t n=0;
  bool ok=false;
  for(; n<1024; ++n) {
    uint8_t buf=uint8_t(n);
    printf("at %08" PRIx32 "\n", base_addr+n);

    ok=spiflash_write(base_addr+n, &buf,1);
    if (!ok)
      printf("could not write at %d\n",n);
    buf=0;
    ok=spiflash_read(base_addr+n, &buf,1);
    
    if (!ok)
      printf("could not read at %d\n",n);
    else {
      printf("READ %d\n",int(buf));
      if (buf != uint8_t(n)) {
	printf("something went wrong\n");
      }
    }
  }

  printf("\n---");
}


extern "C" {
  void user_init(void)
  {
    bar::foo();
    bang();
    uart_set_baud(0, 115200);
  }
}
