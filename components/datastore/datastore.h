/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_DATASTORE_H
#define UNODE_DATASTORE_H

#include "esp_err.h"
#include "common_dmx.h"
#include "common_wifi.h"

void init_datastore(void);
void commit_datastore(void);

esp_err_t get_stored_sta_config(wifi_settings_t *cfg);
void store_sta_config(wifi_settings_t *cfg);

esp_err_t get_stored_dmx_config(dmx_settings_t *cfg);
void store_dmx_config(dmx_settings_t *cfg);


#endif //UNODE_DATASTORE_H
