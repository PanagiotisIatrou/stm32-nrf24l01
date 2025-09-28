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

void nrf24l01_read_pwr_up(nrf24l01 *device, bool *value);

void nrf24l01_write_pwr_up(nrf24l01 *device, bool value);
