/* The classic "blink" example
 *
 * This sample code is in the public domain.
 */
#include <stdlib.h>
#include <dht/dht.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include <string.h>

namespace gpio {
  const int led = 2;
  const int dht = 4;
}

void mainTask(void *pvParameters)
{
  int16_t temperature = 0;
  int16_t humidity = 0;
  
  gpio_enable(gpio::led, GPIO_OUTPUT);
  while(1) {
    if (dht_read_data(DHT_TYPE_DHT11, gpio::dht, &humidity, &temperature)) {
      printf("Humidity: %d%% Temp: %dC\n", 
	     humidity / 10, 
	     temperature / 10);
    } else {
      printf("Could not read data from sensor\n");
    }
    printf("hi\n");
    gpio_write(gpio::led, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_write(gpio::led, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

extern "C" {
  void user_init(void)
  {
    uart_set_baud(0, 115200);
    xTaskCreate(mainTask, "mainTask", 256, NULL, 2, NULL);
  }
}
