/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_FAKE_DNS_H
#define UNODE_FAKE_DNS_H

#define DNS_PORT 53
#define DNS_BUFFER_SIZE 256

void start_fdns_iface(void);
void stop_fdns_iface(void);

#endif //UNODE_FAKE_DNS_H
