#include "register_map.h"

#include "pico/stdlib.h"

void register_map_init(register_map *self, uint8_t csn, spi_inst_t *spi) {
    self->csn = csn;
    self->spi = spi;
}

void register_map_read_register(register_map *self, uint8_t address, uint8_t *result_bytes, uint8_t number_of_bytes) {
    gpio_put(self->csn, 0);

    // Write the command
    uint8_t cmd = 0x00 | address;
    spi_write_blocking(self->spi, &cmd, 1);

    // Read the bytes
    spi_read_blocking(self->spi, 0x00, result_bytes, number_of_bytes);

    gpio_put(self->csn, 1);
}

void register_map_write_register(register_map *self, uint8_t address, uint8_t *bytes, uint8_t number_of_bytes) {
    gpio_put(self->csn, 0);

    // Write the command
    uint8_t cmd = 0x20 | address;
    spi_write_blocking(self->spi, &cmd, 1);

    // Write the bytes
    spi_write_blocking(self->spi, bytes, number_of_bytes);

    gpio_put(self->csn, 1);
}
