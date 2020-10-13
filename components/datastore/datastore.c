/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include "datastore.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "datastore";

static nvs_handle handle;

static const char *label       = "unode";
// values keys
static const char *key_sta_ssid = "ssid";
static const char *key_sta_pswd = "pswd";
static const char *key_a_uni    = "auni";
static const char *key_b_uni    = "buni";
static const char *key_mode     = "mode";

void init_datastore(void) {
    nvs_flash_init();
    nvs_open(label, NVS_READWRITE, &handle);

    ESP_LOGI(TAG, "init complete");
}

void commit_datastore(void) {
    nvs_commit(handle);
}

esp_err_t get_stored_sta_config(wifi_settings_t *cfg) {
    size_t size;

    esp_err_t err = nvs_get_str(handle, key_sta_ssid, cfg->ssid, &size);
    if(err != ESP_OK)
        return err;

    return nvs_get_str(handle, key_sta_pswd, cfg->pswd, &size);
}

void store_sta_config(wifi_settings_t *cfg) {
    nvs_set_str(handle, key_sta_ssid, cfg->ssid);
    nvs_set_str(handle, key_sta_pswd, cfg->pswd);
}

esp_err_t get_stored_dmx_config(dmx_settings_t *cfg) {
    esp_err_t err = nvs_get_u16(handle, key_a_uni, &cfg->port_a_universe);
    if(err != ESP_OK)
        return err;

    err = nvs_get_u16(handle, key_b_uni, &cfg->port_b_universe);
    if(err != ESP_OK)
        return err;

    return nvs_get_u8(handle, key_a_uni, &cfg->mode);
}

void store_dmx_config(dmx_settings_t *cfg) {
    nvs_set_u16(handle, key_a_uni, cfg->port_a_universe);
    nvs_set_u16(handle, key_b_uni, cfg->port_b_universe);
    nvs_set_u8(handle, key_mode, cfg->mode);
}
