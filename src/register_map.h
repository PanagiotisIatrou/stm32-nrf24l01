#pragma once

#include "hardware/spi.h"

typedef struct register_map {
    uint8_t csn;
    spi_inst_t *spi;
} register_map;

void register_map_init(register_map *self, uint8_t csn, spi_inst_t *spi);

void register_map_read_register(register_map *self, uint8_t address, uint8_t *result_bytes, uint8_t number_of_bytes);

void register_map_write_register(register_map *self, uint8_t address, uint8_t *bytes, uint8_t number_of_bytes);
