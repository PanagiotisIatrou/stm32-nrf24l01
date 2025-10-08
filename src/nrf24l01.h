#pragma once

#include "hardware/spi.h"

#include "spi_interface.h"
#include "device_commands.h"

typedef enum {
    DATA_RATE_LOW,
    DATA_RATE_MEDIUM,
    DATA_RATE_HIGH,
} DataRate;

typedef enum {
    POWER_LEVEL_LOW,
    POWER_LEVEL_MEDIUM,
    POWER_LEVEL_HIGH,
    POWER_LEVEL_VERY_HIGH,
} PowerLevel;

typedef struct nrf24l01 {
    spi_interface spi_handler;
    device_commands commands_handler;
} nrf24l01;

bool nrf24l01_init(nrf24l01 *self, uint8_t *address_prefix, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

bool nrf24l01_get_power_state(nrf24l01 *self);

void nrf24l01_power_up(nrf24l01 *self);

void nrf24l01_power_down(nrf24l01 *self);

void nrf24l01_set_pipe0_write(nrf24l01 *self, uint8_t address);

void nrf24l01_set_pipe_read(nrf24l01 *self, uint pipe, uint8_t address);

uint8_t nrf24l01_get_channel(nrf24l01 *self);

void nrf24l01_set_channel(nrf24l01 *self, uint8_t channel);

DataRate nrf24l01_get_data_rate(nrf24l01 *self);

void nrf24l01_set_data_rate(nrf24l01 *self, DataRate data_rate);

PowerLevel nrf24l01_get_power_level(nrf24l01 *self);

void nrf24l01_set_power_level(nrf24l01 *self, PowerLevel power_level);

uint8_t nrf24l01_get_retransmit_delay(nrf24l01 *self);

void nrf24l01_set_retransmit_delay(nrf24l01 *self, uint8_t delay);

uint8_t nrf24l01_get_retransmit_count(nrf24l01 *self);

void nrf24l01_set_retransmit_count(nrf24l01 *self, uint8_t count);

void nrf24l01_send_packets(nrf24l01 *self, uint8_t **value, int count, bool resend_lost_packets);

void nrf24l01_send_packets_no_ack(nrf24l01 *self, uint8_t **value, int count);

void nrf24l01_receive_packets(nrf24l01 *self, uint8_t **packets, int count);

void nrf24l01_receive_packets_inf(nrf24l01 *self, void (*value_callback)(uint8_t* packet));
