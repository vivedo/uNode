/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_ARTNET_H
#define UNODE_ARTNET_H

typedef struct {
    uint16_t universe_a;
    uint8_t *dmx_universe_a;
    uint16_t universe_b;
    uint8_t *dmx_universe_b;
} universes_t;

void start_artnet_server(universes_t *universes);
void stop_artnet_server();

#endif //UNODE_ARTNET_H
