#include "application.h"
#include "debug.h"
#include "time.h"
#include "stdint.h"

#ifdef ARDUINO
#include "Arduino.h"
uint16_t analogRead() {
  return analogRead(0);
}
#endif

#ifdef ESP8266
#include "driver/adc.h"
uint16_t analogRead() {
  uint16_t ret=0;
  adc_read(&ret);
  return ret;
}

#define __ESP_FILE__ __FILE__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "rom/ets_sys.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include <lwip/netdb.h>
#include <sys/socket.h>

#include "private_ssid_config.h"

static const char *TAG = "simple wifi";
const int WIFI_CONNECTED_BIT = BIT0;

#define EXAMPLE_ESP_WIFI_SSID      WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      WIFI_PASS
#define EXAMPLE_MAX_STA_CONN       CONFIG_MAX_STA_CONN

static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  /* For accessing reason codes in case of disconnection */
  system_event_info_t *info = &event->event_info;
    
  switch(event->event_id) {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG, "got ip:%s",
	     ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    break;
  case SYSTEM_EVENT_AP_STACONNECTED:
    ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d",
	     MAC2STR(event->event_info.sta_connected.mac),
	     event->event_info.sta_connected.aid);
    break;
  case SYSTEM_EVENT_AP_STADISCONNECTED:
    ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
	     MAC2STR(event->event_info.sta_disconnected.mac),
	     event->event_info.sta_disconnected.aid);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
    if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
      /*Switch to 802.11 bgn mode */
      esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
    }
    esp_wifi_connect();
    xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    break;
  default:
    break;
  }
  return ESP_OK;
}


void wifi_init_sta()
{
  wifi_event_group = xEventGroupCreate();

  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  wifi_config_t wifi_config = { };
  strcpy((char*)wifi_config.sta.ssid, (const char*)EXAMPLE_ESP_WIFI_SSID);
  strcpy((char*)wifi_config.sta.password, (const char*)EXAMPLE_ESP_WIFI_PASS);
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start() );

  ESP_LOGI(TAG, "wifi_init_sta finished.");
  ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

// POST http://pi:8000/foo/test
// GET http://pi:8000/stats

#define WEB_SERVER "pi"
#define WEB_PORT 8000
#define WEB_URL "http://pi:8000/stats"

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp8266\r\n"
    "\r\n";

static void http_get()
{
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  struct addrinfo *res;
  struct in_addr *addr;
  int s, r;
  char recv_buf[64];

 
  /* Wait for the callback to set the CONNECTED_BIT in the
     event group.
  */
  xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
		      false, true, portMAX_DELAY);
  ESP_LOGI(TAG, "Connected to AP");

  int err = getaddrinfo(WEB_SERVER, "8000", &hints, &res);

  if(err != 0 || res == NULL) {
    ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return;
  }

  /* Code to print the resolved IP.

     Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
  addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
  ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

  s = socket(res->ai_family, res->ai_socktype, 0);
  if(s < 0) {
    ESP_LOGE(TAG, "... Failed to allocate socket.");
    freeaddrinfo(res);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return;
  }
  ESP_LOGI(TAG, "... allocated socket");

  if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
    ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
    close(s);
    freeaddrinfo(res);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    return;
  }

  ESP_LOGI(TAG, "... connected");
  freeaddrinfo(res);

  if (write(s, REQUEST, strlen(REQUEST)) < 0) {
    ESP_LOGE(TAG, "... socket send failed");
    close(s);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    return;
  }
  ESP_LOGI(TAG, "... socket send success");

  struct timeval receiving_timeout;
  receiving_timeout.tv_sec = 5;
  receiving_timeout.tv_usec = 0;
  if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
		 sizeof(receiving_timeout)) < 0) {
    ESP_LOGE(TAG, "... failed to set socket receiving timeout");
    close(s);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    return;
  }
  ESP_LOGI(TAG, "... set socket receiving timeout success");

  /* Read HTTP response */
  do {
    bzero(recv_buf, sizeof(recv_buf));
    r = read(s, recv_buf, sizeof(recv_buf)-1);
    for(int i = 0; i < r; i++) {
      putchar(recv_buf[i]);
    }
  } while(r > 0);

  ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
  close(s);
   
}

#endif

void application::setup() {
#ifdef ARDUINO
  Serial.begin(9600);
  Serial.println("@START");
#endif

#ifdef ESP8266
  adc_config_t config;
  config.mode=ADC_READ_TOUT_MODE;
  config.clk_div=8;
  auto err=adc_init(&config);
  DBG("err=%d\r\n",err);
  wifi_init_sta();
#endif
}


void application::loop()
{
  // TRACE();
  auto a=analogRead();
  DBG("a=%d\r\n",a);
  // time::delay(a);
  // debug::turnBuildinLED(bool(i%2));
  http_get();
  time::delay(50);
}
