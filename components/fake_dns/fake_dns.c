/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include <string.h>
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "fake_dns.h"
#include "common.h"

const char *TAG = "fake_dns";

static TaskHandle_t dns_server_handle = NULL;

static void dns_server_task(void *pvParameters);

/**
 * Starts Fake DNS listener
 * @param universes
 */
void start_fdns_iface(void) {
    if(dns_server_handle != NULL)
        return;

    xTaskCreate(dns_server_task, "fdns_server", 2048, NULL, 2, &dns_server_handle);
    ESP_LOGI(TAG, "started");
}

/**
 * Stops Fake DNS listener
 */
void stop_fdns_iface() {
    if(dns_server_handle == NULL)
        return;

    vTaskDelete(dns_server_handle);
    dns_server_handle = NULL;
}

/**
 * Fake DNS service. Answer 192.168.4.1 (ESP default ip in AP mode) to every request.
 * Used in conjunction with a web server that sends a redirect response to specific uris to trigger captive portals
 *
 * @param pvParameters NULL
 */
static void dns_server_task(void *pvParameters) {
    // appended part of the DNS response
    const uint8_t response[] = { 0xC0, 0x0C,             // offset to the domain name
                                 0x00, 0x01,             // type 1
                                 0x00, 0x01,             // class 1
                                 0x00, 0x00, 0x00, 0x3c, // TTL
                                 0x00, 0x04,             // IP size
                                 192, 168, 4, 1 };       // default ESP ip

    uint8_t buf[DNS_BUFFER_SIZE];

    for(ever) {
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(DNS_PORT);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket binded");

        for(ever) {
            struct sockaddr_in sourceAddr;

            socklen_t socklen = sizeof(sourceAddr);
            int len = recvfrom(sock, buf, DNS_BUFFER_SIZE - sizeof(response), 0, (struct sockaddr *)&sourceAddr, &socklen);

            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            } else {

                // modify received packet into a reply packet and append response
                buf[2]  = 0x81; // change Opcode and flags
                buf[3]  = 0x80;
                buf[6]  = 0x00; // one answer
                buf[7]  = 0x01;
                buf[8]  = 0x00; // NSCOUNT
                buf[9]  = 0x00;
                buf[10] = 0x00; // ARCOUNT
                buf[11] = 0x00;
                memcpy(&(buf[len]), response, sizeof(response));

                sendto(sock, buf, len + sizeof(response), 0, (struct sockaddr *)&sourceAddr, sizeof(struct sockaddr_in));
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}