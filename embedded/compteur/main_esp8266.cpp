#include "application.h"
#include "espressif/esp_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"

void maintask_esp8266(void *pvParameters) {
  while (1) {
    application::loop();
  }
}

extern "C" {
  void user_init(void)
  {
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    //application::setup();
    //xTaskCreate(&maintask_esp8266, "maintask_esp8266", 1024*4, NULL, 2, NULL);
  }
}
