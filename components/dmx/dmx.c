/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include <sys/cdefs.h>
#include <common.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

#include "dmx.h"

static const char *TAG = "dmx";

static TaskHandle_t dmx_sender_handle = NULL;

_Noreturn static void dmx_sender_task(void *pvParameters);

void start_dmx_iface(universes_t *universes) {

    if(dmx_sender_handle != NULL)
        return;

    uart_config_t dmx_config = {
            .baud_rate = 250000,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_2,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &dmx_config);

    uart_disable_rx_intr(UART_NUM_1);
    uart_driver_install(UART_NUM_1, 0, 0, 0, NULL, 0);

    xTaskCreate(dmx_sender_task, "dmx_sender", 1024, universes, 5, NULL);

    ESP_LOGV(TAG, "iface started");
};

void stop_dmx_iface(void) {
    if(dmx_sender_handle == NULL)
        return;

    vTaskDelete(dmx_sender_handle);
    dmx_sender_handle = NULL;

    uart_driver_delete(UART_NUM_1);

    ESP_LOGV(TAG, "iface stopped");
};

/**
 * This task contains busy waitings
 *
 * @param pvParameters
 */
_Noreturn static void dmx_sender_task(void *pvParameters) {
    ESP_LOGV(TAG, "dmx_sender_task started");

    universes_t *universes = pvParameters;

    for(ever) {
        xSemaphoreGive(universes->handling.ack);
        // waits for newdata but keeps sending old data at lower rates when nothing new is received (500ms) to keep signal alive
        xSemaphoreTake(universes->handling.rdy, 500 / portTICK_PERIOD_MS);

        // DMX Packet starts with an >88uS BREAK and an >8uS MAB;
        uart_set_line_inverse(UART_NUM_1, UART_INVERSE_TXD);
        ets_delay_us(100);
        uart_set_line_inverse(UART_NUM_1, UART_INVERSE_DISABLE);
        ets_delay_us(10);

        // Send SC (0x00 for DMX, 0xCC for ArtNet)
        uint8_t DmxStartCode = DMX_START_CODE;
        uart_write_bytes(UART_NUM_1, (const char *) &DmxStartCode, 1);

        // Send universe
        uart_write_bytes(UART_NUM_1, (const char *) universes->buf.a, DMX_UNIVERSE_SIZE);

        uart_wait_tx_done(UART_NUM_1, 10 / portTICK_PERIOD_MS);
    }
};