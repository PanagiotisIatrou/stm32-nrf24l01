#pragma once

#include "hardware/spi.h"

typedef struct {
    spi_inst_t* spi;
    uint8_t mosi;
    uint8_t miso;
    uint8_t sck;
    uint8_t csn;
    uint8_t ce;
} spi_interface;

void spi_interface_init(spi_interface *self, spi_inst_t* spi, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

void spi_interface_send_command(spi_interface *self, uint8_t command, uint8_t *data, uint data_length, uint8_t *output, uint output_length);

void spi_interface_pulse_ce(spi_interface *self);

void spi_interface_enable_ce(spi_interface *self);

void spi_interface_disable_ce(spi_interface *self);
