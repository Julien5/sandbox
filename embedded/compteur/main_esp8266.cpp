#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "application.h"

void hello() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
	 chip_info.cores);
  
  printf("silicon revision %d, ", chip_info.revision);
  
  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
	 (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
  
}

void maintask_esp8266(void *pvParameters) {
  while (1) {
    application::loop();
  }
}

extern "C" {
  void app_main()
  {
    hello();
    application::setup();
    xTaskCreate(&maintask_esp8266, "maintask_esp8266", 1024*16, NULL, 2, NULL);
  }
}
