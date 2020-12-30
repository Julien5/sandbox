#include "common/wifi.h"
#include "common/time.h"
#include "common/rusttypes.h"

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
#include "common/debug.h"

#include "private_ssid_config.h"
#include "common/utils.h"

static const char *TAG = "wifi_esp8266";
const int CONNECTED_BIT = BIT0;

static EventGroupHandle_t wifi_event_group;

static esp_err_t
event_handler(void *ctx, system_event_t *event) {
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;

    switch (event->event_id) {
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
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
        }
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

bool wifi_init_sta_done = false;
void wifi_init_sta() {
    if (wifi_init_sta_done)
        return;
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, (const char *)WIFI_SSID);
    strcpy((char *)wifi_config.sta.password, (const char *)WIFI_PASS);
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_init_sta_done = true;
}

struct UrlReader {
    char *path = {0};
    char host[64] = {0};
    char port[5] = "80";

  public:
    UrlReader(const char *url) {
        // extract "example.com" from "https://example.com:80/blahblahblah"
        //                                     b          2  1
        const char *beg = strstr(url, "//") + 2;
        char *end1 = strstr(beg, "/");
        char *end2 = strchr(beg, ':');
        char *end = end1;

        path = end1;

        if (end2 && end2 < end1) {
            end = end2;
            strncpy(port, end2 + 1, end1 - end2 - 1);
        }
        strncpy(host, beg, end - beg);
        DBG("parts: http://%s:%s/%s\n", host, port, path);
    }
};

int create_socket(const char *url, u8 *errcode) {
    *errcode = 0;
    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    TRACE();

    UrlReader urlReader(url);
    struct addrinfo *res;
    int err = getaddrinfo(urlReader.host, urlReader.port, &hints, &res);
    if (err != 0 || res == NULL) {
        DBG("DNS lookup failed err=%d res=%p", err, res);
        return -1;
    }

    struct in_addr *addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    DBG("DNS lookup succeeded. IP=%s\n", inet_ntoa(*addr));

    int s = socket(res->ai_family, res->ai_socktype, 0);
    *errcode = errno;
    if (s < 0) {
        TRACE();
        ESP_LOGE(TAG, "... socket create failed errno=%d", *errcode);
        freeaddrinfo(res);
        assert(*errcode != 0);
        return -1;
    }
    TRACE();

    err = connect(s, res->ai_addr, res->ai_addrlen);
    *errcode = errno;
    if (err != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", *errcode);
        close(s);
        freeaddrinfo(res);
        return -1;
    }

    TRACE();
    freeaddrinfo(res);
    return s;
}

int write_complete(int s, u8 *data, size_t length) {
    int remain = length;
    u8 *addr = data;
    while (remain > 0) {
        int n = write(s, addr, remain);
        DBG("sent %d bytes\n", n);
        if (n < 0) {
            DBG("failed writing: errno=%d\r\n", errno);
            assert(errno != 0);
            return errno;
        }
        remain -= n;
        addr += n;
    }
    TRACE();
    return 0;
}

size_t
remain(size_t buffer_size, size_t pos) {
    if (pos >= buffer_size)
        return 0;
    return buffer_size - pos;
}

int process_http_request(const char *WEB_URL,
                         const char *request,
                         const u8 *data,
                         size_t data_length,
                         wifi::callback *cb) {
    u8 errcode = 0;
    int s = create_socket(WEB_URL, &errcode);
    if (s < 0) {
        TRACE();
        cb->status(errcode);
        return s;
    }
    UrlReader urlReader(WEB_URL);
    DBG("request:\n%s\n", request);

    auto code = write_complete(s, (u8 *)request, strlen(request));
    if (code != 0) {
        TRACE();
        cb->status(code);
        close(s);
        return code;
    }
    code = write_complete(s, const_cast<u8 *>(data), data_length);
    if (code != 0) {
        TRACE();
        cb->status(code);
        close(s);
        DBG("failed writing: errno=%d\r\n", code);
        return code;
    }

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    code = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout));
    if (code != 0) {
        TRACE();
        cb->status(errno);
        close(s);
        assert(errno != 0);
        return errno;
    }

    /* Read HTTP response */
    u8 buffer[16 * 1024];
    memset(buffer, 0, sizeof(buffer));
    size_t buffer_size = 0;
    while (true) {
        u8 recv_buf[16];
        memset(recv_buf, 0, sizeof(recv_buf));
        int r = read(s, recv_buf, sizeof(recv_buf));
        if (r > 0) {
            if (buffer_size + r >= sizeof(buffer))
                break; // max size exceeded
            memcpy(buffer + buffer_size, recv_buf, r);
            buffer_size += r;
        } else {
            DBG("done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
            // note: EAGAIN (11) seems to signal end of stream.
            if (errno == EWOULDBLOCK) {
                // TODO: wait a little and retry until timeout.
            }
            break;
        }
    }
    close(s);

    cb->status(0);
    cb->data_length(buffer_size);

    // send chunk-wise so that arduino read buffer does not overflow.
    if (cb && buffer_size > 0) {
        u8 buf[32] = {0};
        size_t pos = 0;
        while (remain(buffer_size, pos) > 0) {
            size_t size_copy = xMin(sizeof(buf), remain(buffer_size, pos));
            memcpy(buf, buffer + pos, size_copy);
            cb->data((u8 *)buf, size_copy);
            pos += size_copy;
            common::time::delay(100);
        }
    }

    return 0;
}

int get(const char *WEB_URL, wifi::callback *cb) {
    UrlReader urlReader(WEB_URL);
    char request[256] = {0};
    snprintf(request, sizeof(request), "GET %s HTTP/1.0\r\n"
                                       "Host: %s\n"
                                       "User-Agent: esp-idf/1.0 esp8266\r\n"
                                       "\r\n",
             urlReader.path,
             urlReader.host);
    return process_http_request(WEB_URL, request, 0, 0, cb);
}

int post(const char *WEB_URL, const u8 *data, size_t data_length, wifi::callback *cb) {
    // > POST /foo/test HTTP/1.1
    // > Host: localhost:8000
    // > User-Agent: curl/7.58.0
    // > Accept: */*
    // > Content-Length: 196
    // > Content-Type: application/x-www-form-urlencoded
    UrlReader urlReader(WEB_URL);
    char request[256] = {0};
    snprintf(request, sizeof(request), "POST %s HTTP/1.1\r\n"
                                       "Host: %s\n"
                                       "User-Agent: esp-idf/1.0 esp8266\r\n"
                                       "Accept: */*\r\n"
                                       "Content-Length: %d\r\n"
                                       "Content-Type: application/octet-stream\r\n"
                                       "\r\n",
             urlReader.path,
             urlReader.host,
             data_length);
    utils::dump(data, data_length);
    return process_http_request(WEB_URL, request, data, data_length, cb);
}

namespace wifi {
    wifi::wifi() {
        wifi_init_sta();
    }

    wifi::~wifi() {
    }

    int wifi::post(const char *url, const u8 *data, const u16 Ldata, callback *r) {
        //const char * server = "pi";
        //const char * port = "8000";
        // return http_post(server,port,req,data,Ldata);
        utils::dump(data, Ldata);
        return ::post(url, data, Ldata, r);
    }
    int wifi::get(const char *url, callback *r) {
        //const char * server = "pi";
        //const char * port = "8000";
        // return http_post(server,port,req,data,Ldata);
        return ::get(url, r);
    }
}
