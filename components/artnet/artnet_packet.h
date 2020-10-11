/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */


#ifndef UNODE_ARTNET_PACKET_H
#define UNODE_ARTNET_PACKET_H

#include "artnet_vars.h"

typedef union {
    struct { /* ArtDmx packet */
        char ID[8];
        uint16_t opcode;            // LE

        union {
            struct {
                uint16_t protocol_version;  // BE
                uint8_t sequence;
                uint8_t physical;
                uint16_t universe;          // LE
                uint16_t data_length;       // BE
                uint8_t data[512];
            } __attribute__((packed)) ArtDmx;

            struct {
                uint16_t protocol_version;  // BE
                uint8_t sequence;
                uint8_t start_code;
                uint16_t universe;          // LE
                uint16_t data_length;       // BE
                uint8_t data[512];
            } __attribute__((packed)) ArtNzs;

            struct {
                uint16_t protocol_version;  // BE
                uint8_t talk_to_me;
                uint8_t priority;
            } __attribute__((packed)) ArtPoll;

            struct {
                uint32_t ip_address;
                uint16_t port_number;
                uint16_t vers_info;
                uint16_t swtc;
                uint16_t oem;
                uint8_t ueba;
                uint8_t status1;
                uint16_t esta;
                char short_name[18];
                char long_name[64];
                char node_report[64];
                uint16_t num_ports;
                uint8_t port_types[4];
                uint8_t good_input[4];
                uint8_t good_output[4];
                uint8_t sw_in[4];
                uint8_t sw_out[4];
                uint8_t sw_video; /* deprecated */
                uint8_t sw_macro; /* deprecated */
                uint8_t sw_remote; /* deprecated */
                uint8_t _spare1;
                uint8_t _spare2;
                uint8_t _spare3;
                uint8_t style;
                uint8_t mac[6];
                uint32_t bind_ip;
                uint8_t bind_index;
                uint8_t status2;
                uint8_t _filler[26];
            } __attribute__((packed)) ArtPollReply;
        };

    } __attribute__((packed));

    uint8_t raw[ARTNET_PACKET_SIZE];
} artnet_packet_t;

#endif //UNODE_ARTNET_PACKET_H
