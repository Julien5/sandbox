#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void serial(void *pvParameters) {
  while (1) {
    application::loop_serial();
  }
}

void wifi(void *pvParameters) {
  while (1) {
    application::loop_wifi();
  }
}

extern "C" {
  void app_main()
  {
    application::setup();
    xTaskCreate(&serial, "serial_task", 1024*16, nullptr, 2, nullptr);
    xTaskCreate(&wifi, "wifi_task", 1024*16, nullptr, 2, nullptr);
  }
}
