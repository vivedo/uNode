/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */


#ifndef UNODE_ARTNET_VARS_H
#define UNODE_ARTNET_VARS_H

#define ARTNET_PORT                 6454
#define ARTNET_PACKET_SIZE          530
#define ARTNET_ARTPOLLREPLY_SIZE    240

// OpCodes
#define ARTNET_ARTPOLL              0x2000
#define ARTNET_ARTPOLL_REPLY        0x2100
#define ARTNET_DIAG_DATA            0x2300
#define ARTNET_COMMAND              0x2400
#define ARTNET_ARTDMX               0x5000
#define ARTNET_NZS                  0x5100
#define ARTNET_SYNC                 0x5200
#define ARTNET_ADDRESS              0x6000
#define ARTNET_INPUT                0x7000
#define ARTNET_TOD_REQUEST          0x8000
#define ARTNET_TOD_DATA             0x8100
#define ARTNET_TOD_CONTROL          0x8200
#define ARTNET_RDM                  0x8300
#define ARTNET_RDM_SUB              0x8400
#define ARTNET_FIRMWARE_MASTER      0xF200
#define ARTNET_FIRMWARE_REPLY       0xF300
#define ARTNET_IP_PROG              0xF800
#define ARTNET_IP_PROG_REPLY        0xF900

#endif //UNODE_ARTNET_VARS_H
