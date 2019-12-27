#include <stdlib.h>
#include <dht/dht.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include <string.h>

#include "http.h"

#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_system.h>

namespace gpio {
  const int led = 2;
  const int dht = 4;
}

#define SLEEP_1HR 0xD693A400
#define SLEEP_10SECS 10*1000*1000

static void deep_sleep()
{
    /* Clean all network connections */
    sdk_wifi_station_disconnect();

    /* Now just wait for the RTC to kill the CPU core */
    sdk_system_deep_sleep(SLEEP_10SECS); 
}


void getCallback(uint8_t *data, const int16_t length) {
  printf("receveid %d bytes\n", length);
}

void mainTask(void *pvParameters)
{
  int16_t temperature = 0;
  int16_t humidity = 0;
  
  gpio_enable(gpio::led, GPIO_OUTPUT);
  while(1) {
    char log[128]={0};
    if (dht_read_data(DHT_TYPE_DHT11, gpio::dht, &humidity, &temperature)) {
      snprintf(log, 128,
	       "Humidity: %d%% Temp: %dC", 
	       humidity / 10, 
	       temperature / 10
	       );
      printf("%s",log);
    } else {
      printf("Could not read data from sensor\n");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // http::get(getCallback);
    http::post((uint8_t*)log,strlen(log),getCallback);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("done\n");
    deep_sleep();
  }
}

extern "C" {
  void user_init(void)
  {
    uart_set_baud(0, 115200);
    xTaskCreate(mainTask, "mainTask", 1024, NULL, 2, NULL);
  }

}
