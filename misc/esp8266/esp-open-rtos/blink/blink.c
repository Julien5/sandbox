/* The classic "blink" example
 *
 * This sample code is in the public domain.
 */
#include <stdlib.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include <string.h>

const int gpio = 2;

void blinkenTask(void *pvParameters)
{
  gpio_enable(gpio, GPIO_OUTPUT);
  while(1) {
    printf("hi\n");
    gpio_write(gpio, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_write(gpio, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void app_main(void *pvParameters)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
            chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}


/* This task uses the high level GPIO API (esp_gpio.h) to blink an LED.
 *
 */
void user_init(void)
{
  uart_set_baud(0, 115200);
  xTaskCreate(blinkenTask, "blinkenTask", 256, NULL, 2, NULL);
  //xTaskCreate(blinkenRegisterTask, "blinkenRegisterTask", 256, NULL, 2, NULL);
}
