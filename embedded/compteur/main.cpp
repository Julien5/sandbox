#ifdef DEVHOST
int main(int argc, char ** argv) {
   return 0;
}
#endif

#ifdef ESP8266
#include "application_esp8266.h"
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
extern "C" {
  void user_init(void)
  {
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    xTaskCreate(&maintask, "maintask", 1024, NULL, 2, NULL);
  }
}
#endif
