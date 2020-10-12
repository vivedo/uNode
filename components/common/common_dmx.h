/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_COMMON_DMX_H
#define UNODE_COMMON_DMX_H

#define UNODE_MAX_UNIVERSES 2
#define UNODE_UNIVERSE_A 0
#define UNODE_UNIVERSE_B 1

#define DMX_UNIVERSE_SIZE 512

#define DMX_START_CODE 0x00
#define RDM_START_CODE 0xCC

typedef struct {
    SemaphoreHandle_t rdy; // new data ready
    SemaphoreHandle_t ack; // new data ack (read and disposable)
} universes_handling_t ;

typedef struct {
    uint16_t universe;
    uint8_t dmx[DMX_UNIVERSE_SIZE];
} universe_t;

typedef struct {
    universe_t u[UNODE_MAX_UNIVERSES];
    universes_handling_t handling;
} universes_t;

#endif //UNODE_COMMON_DMX_H
