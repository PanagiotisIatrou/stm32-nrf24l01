#pragma once

#include "hardware/spi.h"

uint8_t spi0_mosi_pins[3] = {3, 7, 19};
uint8_t spi0_miso_pins[3] = {0, 4, 16};
uint8_t spi0_sck_pins[3] = {2, 6, 18};

uint8_t spi1_mosi_pins[2] = {11, 15};
uint8_t spi1_miso_pins[2] = {8, 12};
uint8_t spi1_sck_pins[2] = {10, 14};

spi_inst_t* pins_to_spi(uint8_t mosi, uint8_t miso, uint8_t sck) {
    if ((mosi == spi0_mosi_pins[0] || mosi == spi0_mosi_pins[1] || mosi == spi0_mosi_pins[2])
        && (miso == spi0_miso_pins[0] || miso == spi0_miso_pins[1] || miso == spi0_miso_pins[2])
        && (sck == spi0_sck_pins[0] || sck == spi0_sck_pins[1] || sck == spi0_sck_pins[2])
    ) {
        return spi0;
    } else if ((mosi == spi1_mosi_pins[0] || mosi == spi1_mosi_pins[1])
        && (miso == spi1_miso_pins[0] || miso == spi1_miso_pins[1])
        && (sck == spi1_sck_pins[0] || sck == spi1_sck_pins[1])
    ) {
        return spi1;
    } else{
        return NULL;
    }
}
