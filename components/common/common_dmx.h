/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_COMMON_DMX_H
#define UNODE_COMMON_DMX_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define DMX_UNIVERSE_SIZE 512
#define DMX_START_CODE 0x00
#define RDM_START_CODE 0xCC

#define DMX_PROTOCOL_ARTNET 0
#define DMX_PROTOCOL_SACN 1
#define DMX_OUTPUT 0
#define DMX_INPUT 1

typedef struct {
    SemaphoreHandle_t rdy; // new data ready
    SemaphoreHandle_t ack; // new data ack (read and disposable)
} universes_handling_t ;

typedef struct {
    uint16_t port_a_universe;
    uint16_t port_b_universe;

    union {
        struct {
            uint8_t protocol : 1;
            uint8_t port_a_direction : 1;
            uint8_t port_b_direction : 1;
        };
        uint8_t mode;
    };
} dmx_settings_t ;

typedef unsigned char dmx_value_t;

typedef struct {
    dmx_value_t a[DMX_UNIVERSE_SIZE];
    dmx_value_t b[DMX_UNIVERSE_SIZE];
} dmx_buffers_t;

typedef struct {
    dmx_settings_t settings;
    dmx_buffers_t buf;
    universes_handling_t handling;
} universes_t;

#endif //UNODE_COMMON_DMX_H
