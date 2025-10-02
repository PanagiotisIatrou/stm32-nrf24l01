#pragma once

#include "hardware/spi.h"
#include "register_map.h"

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
    uint8_t mosi;
    uint8_t miso;
    uint8_t sck;
    uint8_t csn;
    uint8_t ce;
    spi_inst_t* spi;

    register_map register_map;
} nrf24l01;

static bool initialized_spi0 = false;
static bool initialized_spi1 = false;

bool nrf24l01_init(nrf24l01 *device, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

bool nrf24l01_get_power_state(nrf24l01 *device);

void nrf24l01_power_up(nrf24l01 *device);

void nrf24l01_power_down(nrf24l01 *device);

void nrf24l01_config_tx(nrf24l01 *device, uint8_t *value);

void nrf24l01_config_rx(nrf24l01 *device, uint8_t *value);

uint8_t nrf24l01_get_channel(nrf24l01 *device);

void nrf24l01_set_channel(nrf24l01 *device, uint8_t channel);

void nrf24l01_set_data_rate(nrf24l01 *device, DataRate data_rate);

void nrf24l01_set_power_level(nrf24l01 *device, PowerLevel power_level);

void nrf24l01_set_retransmit_delay(nrf24l01 *device, uint8_t delay);

void nrf24l01_set_retransmit_count(nrf24l01 *device, uint8_t count);

void nrf24l01_send_packets(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_send_packets_fast(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_send_packets_no_ack(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_send_packets_no_ack_fast(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_receive_packet(nrf24l01 *device);
