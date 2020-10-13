/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include <string.h>
#include "sys/param.h"
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "artnet.h"
#include "artnet_packet.h"
#include "common.h"

static const char *TAG = "artnet";

static TaskHandle_t artnet_server_handle = NULL;

static void artnet_server_task(void *pvParameters);
static void process_artnet_packet(artnet_packet_t *packet, universes_t *universes, int socket, struct sockaddr_in *sourceAddr);
static void fix_artnet_packet(artnet_packet_t *packet);
static void storeDmxUniverse(artnet_packet_t *packet, dmx_value_t *universe);
static void answerArtPoll(artnet_packet_t *packet, int socket, struct sockaddr_in *sourceAddr);

/**
 * Starts ArtNet listener
 * @param universes
 */
void start_artnet_iface(universes_t *universes) {
    if(artnet_server_handle != NULL)
        return;

    xTaskCreate(artnet_server_task, "artnet_server", 4096, universes, 6, &artnet_server_handle);

    ESP_LOGI(TAG, "started");
}

/**
 * Stops ArtNet listener
 */
void stop_artnet_iface() {
    if(artnet_server_handle != NULL) {
        vTaskDelete(artnet_server_handle);
        artnet_server_handle = NULL;
    }
}

/**
 * FreeRTOS task containing ArtNet UDP server listener
 *
 * @param pvParameters universes_t struct containing universes buffers
 */
static void artnet_server_task(void *pvParameters) {
    artnet_packet_t packetBuffer;
    //char addr_str[16];

    for(ever) {
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(ARTNET_PORT);
        //inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket binded");

        for(ever) {
            //ESP_LOGI(TAG, "Waiting for data");

            struct sockaddr_in sourceAddr;

            socklen_t socklen = sizeof(sourceAddr);
            int len = recvfrom(sock, packetBuffer.raw, sizeof(packetBuffer.raw) - 1, 0, (struct sockaddr *)&sourceAddr, &socklen);

            if (len < 0) { // Error occured during receiving
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            } else { // Data received

                process_artnet_packet(&packetBuffer, (universes_t *) pvParameters, sock, &sourceAddr);
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

/**
 * Processes received ArtNet packet
 *
 * @param packet received packet
 * @param universes destination universes buffers
 * @param socket UDP socket used for replys
 * @param sourceAddr address of sender
 */
static void process_artnet_packet(artnet_packet_t *packet, universes_t *universes, int socket, struct sockaddr_in *sourceAddr) {
    fix_artnet_packet(packet);

    // if(strcmp(packet->ID, "Art-Net") != 0) {
    if(packet->ID[0] != 'A') { // Should check if first 8 bytes are equal to 'Art-Net\0' but checking just first one is enough and faster
        ESP_LOGI(TAG, "Received invalid packet");
        return;
    }

    switch(packet->opcode) {
        case ARTNET_ARTDMX:
            ESP_LOGV(TAG, "Received ArtDmx (protocol %d) packet of universe %d, size %d", packet->ArtDmx.protocol_version, packet->ArtDmx.universe, packet->ArtDmx.data_length);

            // if A and B output the same universe use only one buffer, and handle output on 2 port from same buffer in component/dmx
            if (packet->ArtDmx.universe == universes->settings.port_a_universe &&
                universes->settings.port_a_direction == DMX_OUTPUT) {

                xSemaphoreTake(universes->handling.ack, portMAX_DELAY);
                storeDmxUniverse(packet, universes->buf.a);
                xSemaphoreGive(universes->handling.rdy);

            } else if (packet->ArtDmx.universe == universes->settings.port_b_universe &&
                       universes->settings.port_b_direction == DMX_OUTPUT) {

                xSemaphoreTake(universes->handling.ack, portMAX_DELAY);
                storeDmxUniverse(packet, universes->buf.b);
                xSemaphoreGive(universes->handling.rdy);
            }

            break;
        case ARTNET_ARTPOLL:
            ESP_LOGV(TAG, "Received ArtPoll (protocol %d) packet", packet->ArtPoll.protocol_version);

            answerArtPoll(packet, socket, sourceAddr);
            break;
    }
}

/**
 * Fixes byte order in some received ArtNet packet fields (ArtNet uses different endianness for different fields)
 *
 * @param packet received packet
 */
static void fix_artnet_packet(artnet_packet_t *packet) {
    switch(packet->opcode) {
        case ARTNET_ARTDMX:
        case ARTNET_NZS:
            packet->ArtDmx.data_length = ntohs(packet->ArtDmx.data_length); // ArtDmx and ArtNzs data_length are the same byte
        case ARTNET_ARTPOLL:
            packet->ArtDmx.protocol_version = ntohs(packet->ArtDmx.protocol_version); // ArtPoll, ArtDmx and ArtNzs protocol_version are the same byte
            break;
    }
}

/**
 * Copies received ArtDmx packet into a given dmx universe buffer, considering ArtDmx length
 *
 * @param packet received packet
 * @param universes destination universe
 */
static void storeDmxUniverse(artnet_packet_t *packet, dmx_value_t *universe) {
    memcpy(universe, packet->ArtDmx.data, MIN(packet->ArtDmx.data_length, DMX_UNIVERSE_SIZE));
}

/**
 * Builds and sends an ArtPollReply to sourceAddr through socket.
 * Destroys incoming ArtPoll packets as it uses packet as buffer to build ArtPollReply response.
 *
 * @param packet packet buffer
 * @param socket UDP socket used to send ArtPollReply
 * @param sourceAddr address asking for an ArtPollReply
 */
static void answerArtPoll(artnet_packet_t *packet, int socket, struct sockaddr_in *sourceAddr) {
    // Clear buffer for ArtPollReply
    memset(packet->raw, 0, ARTNET_ARTPOLLREPLY_SIZE);

    // Get STATION IP address, as ArtNet server is not enabled in AP mode
    tcpip_adapter_ip_info_t ip;
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip);

    // Get STATION MAC address, as ArtNet server is not enabled in AP mode
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    // Build ArtPollReply
    strcpy(packet->ID, "Art-Net");
    packet->opcode = ARTNET_ARTPOLL_REPLY;
    packet->ArtPollReply.ip_address = ip.ip.addr;
    packet->ArtPollReply.bind_ip = ip.ip.addr;
    packet->ArtPollReply.port_number = ARTNET_PORT;
    memcpy(packet->ArtPollReply.mac, mac, 6);
    sprintf(packet->ArtPollReply.short_name, "µNode %02X%02X%02X", mac[3], mac[4], mac[5]);
    sprintf(packet->ArtPollReply.long_name, "vivedo µNode - %02X:%02X:%02X:%02X:%02X:%02X @ %s",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ip4addr_ntoa(&ip.ip));

    packet->ArtPollReply.status1 = 0b11000010; // Front panel 'normal' & RDM support
    packet->ArtPollReply.status2 = 0b00001111; // Artnet >=3, DHCP and Web Panel support
    packet->ArtPollReply.oem = packet->ArtPollReply.esta = 0xffff;

    packet->ArtPollReply.num_ports = htons(2);
    memset(packet->ArtPollReply.port_types, 0b11000000, 2);

    // Sends packet
    int err = sendto(socket, packet->raw, ARTNET_ARTPOLLREPLY_SIZE, 0, (struct sockaddr *)sourceAddr, sizeof(struct sockaddr_in));
    if (err < 0)
        ESP_LOGE(TAG, "Error occurred during custom sending: errno %d", errno);
}
