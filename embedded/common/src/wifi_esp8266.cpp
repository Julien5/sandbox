#include "wifi.h"

#define __ESP_FILE__ __FILE__
#include <string>
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
#include "debug.h"

#include "private_ssid_config.h"

static const char *TAG = "wifi_esp8266";
const int CONNECTED_BIT = BIT0;

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
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
        if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
            /*Switch to 802.11 bgn mode */
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
        }
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifi_init_sta()
{
  tcpip_adapter_init();
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  wifi_config_t wifi_config={};
  strcpy((char*)wifi_config.sta.ssid, (const char*)WIFI_SSID);
  strcpy((char*)wifi_config.sta.password, (const char*)WIFI_PASS);
  ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
  ESP_ERROR_CHECK( esp_wifi_start() );
}

namespace Method {
  enum Method {
    get,
    post
  };
  char const * names[2] = {
    "GET",
    "POST"
  };
}

int http(Method::Method method, const char * WEB_URL, wifi::callback *r_cb)
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
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
		      false, true, portMAX_DELAY);
  ESP_LOGI(TAG, "Connected to AP");

  char schema[6]={0};
  char * path = {0};
  char host[64]={0};
  char port[5]="80";
  {
    // extract "example.com" from "https://example.com:80/blahblahblah"
    //                                     b          2  1
    const char * beg=strstr(WEB_URL,"//")+2;
    char * end1=strstr(beg,"/");
    char * end2=strchr(beg,':');
    char * end=end1;

    path = end1;
    
    if (end2 && end2<end1) {
      end=end2;
      strncpy(port,end2+1,end1-end2-1);
    }
    strncpy(host,beg,end-beg);
    strncpy(schema,WEB_URL,beg-WEB_URL-3);
  }
  DBG("parts: %s://%s:%s/%s\n",schema,host,port,path);
 
  char url[128]={0};
  // do not include the port
  snprintf(url, 128,"%s://%s/%s",schema,host,path);
  
  DBG("recomposed: %s\n",url);
  int err = getaddrinfo(host, port, &hints, &res);

  if(err != 0 || res == NULL) {
    ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return -1;
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
    return -1;
  }
  ESP_LOGI(TAG, "... allocated socket");

  if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
    ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
    close(s);
    freeaddrinfo(res);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    return -1;
  }

  ESP_LOGI(TAG, "... connected");
  freeaddrinfo(res);  
  char request[256]={0};
  snprintf(request, 256,
	   "%s %s HTTP/1.0\r\n"
	   "Host: %s\n"
	   "User-Agent: esp-idf/1.0 esp8266\r\n"
	   "\r\n",
	   Method::names[method],
	   path,
	   host
	   );
  DBG("request:\n%s\n",request);
  
  if (write(s, request, strlen(request)) < 0) {
    ESP_LOGE(TAG, "... socket send failed");
    close(s);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    return -1;
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
    return -1;
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
  return 0;
}

namespace wifi {
  wifi::wifi() {
    wifi_init_sta();
  }

  wifi::~wifi() {
  }
 
  int wifi::post(const char* url, const uint8_t * data, const int Ldata, callback * r) {
    //const char * server = "pi";
    //const char * port = "8000";
    // return http_post(server,port,req,data,Ldata);
    return http(Method::post,url,nullptr);
  }
  int wifi::get(const char* url, callback * r) {
    //const char * server = "pi";
    //const char * port = "8000";
    // return http_post(server,port,req,data,Ldata);
    return http(Method::get,url,r);
  }
}
