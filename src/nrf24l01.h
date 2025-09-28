#pragma once

#include "pico/stdlib.h"

typedef struct nrf24l01 {
    uint8_t mosi;
    uint8_t miso;
    uint8_t sck;
    uint8_t csn;
    uint8_t ce;
} nrf24l01;

nrf24l01 nrf24l01_init(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

uint8_t nrf24l01_read_config(nrf24l01 device);

void nrf24l01_write_config(nrf24l01 device, uint8_t value);
