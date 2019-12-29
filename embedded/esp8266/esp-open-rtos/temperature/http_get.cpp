/* http_get - Retrieves a web page over HTTP GET.
 *
 * See http_get_ssl for a TLS-enabled version.
 *
 * This sample code is in the public domain.,
 */
#include "http.h"

#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ssid_config.h"

#define WEB_SERVER "pi"
#define WEB_PORT "8000"
#define WEB_PATH "/get"

void http::get(http::get_callback getCallback)
{
    int successes = 0, failures = 0;
    printf("HTTP get task starting...\r\n");

    while(1) {
      struct addrinfo hints{};
      hints.ai_family=AF_UNSPEC;
      hints.ai_socktype=SOCK_STREAM;
      
        struct addrinfo *res;

        printf("Running DNS lookup for %s...\r\n", WEB_SERVER);
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if (err != 0 || res == NULL) {
            printf("DNS lookup failed err=%d res=%p\r\n", err, res);
            if(res)
                freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }

#if LWIP_IPV6
        {
            struct netif *netif = sdk_system_get_netif(0);
            int i;
            for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                printf("  ip6 %d state %x\n", i, netif_ip6_addr_state(netif, i));
                if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)))
                    printf("  ip6 addr %d = %s\n", i, ip6addr_ntoa(netif_ip6_addr(netif, i)));
            }
        }
#endif

        struct sockaddr *sa = res->ai_addr;
        if (sa->sa_family == AF_INET) {
            printf("DNS lookup succeeded. IP=%s\r\n", inet_ntoa(((struct sockaddr_in *)sa)->sin_addr));
        }
#if LWIP_IPV6
        if (sa->sa_family == AF_INET6) {
            printf("DNS lookup succeeded. IP=%s\r\n", inet6_ntoa(((struct sockaddr_in6 *)sa)->sin6_addr));
        }
#endif

        int s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            printf("... Failed to allocate socket.\r\n");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }

        printf("... allocated socket\r\n");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            close(s);
            freeaddrinfo(res);
            printf("... socket connect failed.\r\n");
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }

        printf("... connected\r\n");
        freeaddrinfo(res);

        const char *req =
            "GET "WEB_PATH" HTTP/1.1\r\n"
            "Host: "WEB_SERVER"\r\n"
            "User-Agent: esp-open-rtos/0.1 esp8266\r\n"
            "Connection: close\r\n"
            "\r\n";
        if (write(s, req, strlen(req)) < 0) {
            printf("... socket send failed\r\n");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }
        printf("... socket send success\r\n");

        static uint8_t recv_buf[128];
        int16_t r;
        do {
            bzero(recv_buf, 128);
            r = read(s, recv_buf, 127);
            if(r > 0) {
                printf("%s", recv_buf);
            }
	    getCallback(recv_buf,r);
        } while(r > 0);

        printf("... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        if(r != 0)
            failures++;
        else
            successes++;
        close(s);
        printf("successes = %d failures = %d\r\n", successes, failures);
	return;
    }
}
