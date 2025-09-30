#pragma once

#include "hardware/spi.h"
#include "register_map.h"

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

void nrf24l01_power_up(nrf24l01 *device);

void nrf24l01_set_as_primary_tx(nrf24l01 *device);

void nrf24l01_set_as_primary_rx(nrf24l01 *device);

void nrf24l01_config_tx(nrf24l01 *device, uint8_t *value);

void nrf24l01_config_rx(nrf24l01 *device, uint8_t *value);

void nrf24l01_send_packets(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_send_packets_fast(nrf24l01 *device, uint8_t **value, int count);

void nrf24l01_receive_packet(nrf24l01 *device, uint8_t *value);

void nrf24l01_start_listening(nrf24l01 *device);

void nrf24l01_stop_listening(nrf24l01 *device);
