//
// Created by vivedo on 13/10/20.
//

#ifndef UNODE_COMMON_WIFI_H
#define UNODE_COMMON_WIFI_H

#include "esp_wifi_types.h"

typedef union {
    struct {
        char ssid[32];
        char pswd[64];
    };
    wifi_ap_config_t ap;
    wifi_sta_config_t sta;
} wifi_settings_t ;

#endif //UNODE_COMMON_WIFI_H
