#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <dht/dht.h>

namespace gpio {
  const int led = 2;
  const int dht = 4;
}


void temperature_task(void *arg)
{
  ESP_ERROR_CHECK(dht_init(GPIO_NUM_4, false));
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  while (1)
    {
      int16_t humidity = 0;
      int16_t temperature = 0;
      if (dht_read_data(DHT_TYPE_DHT22, GPIO_NUM_4, &humidity, &temperature) == ESP_OK) {
	// e.g. in dht22, 604 = 60.4%, 252 = 25.2 C
	// If you want to print float data, you should run `make menuconfig`
	// to enable full newlib and call dht_read_float_data() here instead
	printf("Humidity: %d Temperature: %d\n", humidity, temperature);
      } else {
	printf("Fail to get dht temperature data\n");
      }
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  vTaskDelete(NULL);
}

extern "C" {
  void app_main()
  {
    xTaskCreate(temperature_task, "temperature task", 2048, NULL, tskIDLE_PRIORITY, NULL);
  }
}
