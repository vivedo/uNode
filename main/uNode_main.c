/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "webserver.h"
#include "artnet.h"
#include "common_dmx.h"
#include "string.h"
#include "dmx.h"
#include "datastore.h"
#include "fake_dns.h"

static const char *TAG = "uNode";

universes_t universes;

static void start_auto_mode(void);
static void set_setup_mode(void);
static void set_run_mode(wifi_settings_t *sta_settings);
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void init_dmx_buffers(void);

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());

    init_datastore();

    // Init DMX immediately to send an empty universe on DMX
    init_dmx_buffers();
    start_dmx_iface(&universes);

    tcpip_adapter_init();
    esp_event_loop_init(event_handler, NULL);

    // setup & start wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    wifi_settings_t sta_settings;
    if(get_stored_sta_config(&sta_settings) == ESP_OK)
        set_run_mode(&sta_settings);
    else
        set_setup_mode();
}

static void set_setup_mode(void) {
    ESP_LOGI(TAG, "Entering SETUP MODE");
    stop_artnet_iface();
    stop_webserver();
    esp_wifi_stop();

    wifi_config_t wifi_config = {
            .ap = {
                    .ssid_len = 12,
                    .max_connection = 5,
                    .authmode = WIFI_AUTH_OPEN,
            },
    };

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    sprintf((char*)wifi_config.ap.ssid, "uNode-%02X%02X%02X", mac[3], mac[4], mac[5]);

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();

    start_fdns_iface();

    start_webserver();
}

static void set_run_mode(wifi_settings_t *sta_settings) {
    ESP_LOGI(TAG, "Entering RUN MODE");

    stop_fdns_iface();
    stop_artnet_iface();
    stop_webserver();
    esp_wifi_stop();

    wifi_config_t wifi_config = {
            .sta = sta_settings->sta,
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    start_fdns_iface();

    start_webserver();
    start_artnet_iface(&universes);
}

static void init_dmx_buffers(void) {
    if(get_stored_dmx_config(&universes.settings) != ESP_OK) {
        universes.settings.port_a_universe = 0;
        universes.settings.port_b_universe = 1;
        universes.settings.mode = 0x00; // Mode ArtNet, A&B output
        store_dmx_config(&universes.settings);
    }

    memset(universes.buf.a, 0x00, DMX_UNIVERSE_SIZE);
    memset(universes.buf.b, 0x00, DMX_UNIVERSE_SIZE);
    universes.handling.ack = xSemaphoreCreateBinary();
    universes.handling.rdy = xSemaphoreCreateBinary();
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