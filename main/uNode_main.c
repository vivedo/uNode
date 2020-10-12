/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <webserver.h>
#include <artnet.h>
#include <common_dmx.h>
#include <string.h>
#include <dmx.h>

// Contains temporary wifi STA settings, to keep them out of vcs, will be removed wen wifi manager will be implemented
#include "_tmp_wifi_settings.h" // TODO: remove

static const char *TAG = "main";

universes_t universes;

static void initialise_wifi(void);
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void start_net_functions(void);
static void stop_net_functions(void);

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());

    // Init DMX immediately to sendempty universe on DMX
    universes.u[UNODE_UNIVERSE_A].universe = 0;
    universes.u[UNODE_UNIVERSE_B].universe = 1;
    memset(universes.u[UNODE_UNIVERSE_A].dmx, 0x00, DMX_UNIVERSE_SIZE);
    memset(universes.u[UNODE_UNIVERSE_B].dmx, 0x00, DMX_UNIVERSE_SIZE);
    universes.handling.ack = xSemaphoreCreateBinary();
    universes.handling.rdy = xSemaphoreCreateBinary();
    start_dmx_iface(&universes);


    initialise_wifi();
}

static void initialise_wifi(void) {
    tcpip_adapter_init();

    esp_event_loop_init(event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = WIFI_SSID,
                    .password = WIFI_PSWD,
            },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    system_event_info_t *info = &event->event_info;

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            ESP_ERROR_CHECK(esp_wifi_connect());

            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "Got IP: '%s'", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

            start_net_functions();

            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);

            if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) // Switch to 802.11 bgn mode
                esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

            ESP_ERROR_CHECK(esp_wifi_connect());

            break;
        default:
            break;
    }
    return ESP_OK;
}

static void start_net_functions(void) {
    start_webserver();
    start_artnet_iface(&universes);
};

static void stop_net_functions(void) {
    stop_artnet_iface();
    stop_webserver();
};