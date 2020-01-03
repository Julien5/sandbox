#include "application.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"

#include <esp_system.h>
#include <esp_log.h>

void maintask_esp8266(void *pvParameters) {
  while (1) {
    application::loop();
  }
}

extern "C" {
  void user_init(void)
  {
    ESP_LOGI("TEST", "SDK factory test firmware version:%s\n", esp_get_idf_version());

    //application::setup();
    //xTaskCreate(&maintask_esp8266, "maintask_esp8266", 1024*4, NULL, 2, NULL);
  }
  void app_main()
  {
    printf("Hello world!\n");
  }
}
