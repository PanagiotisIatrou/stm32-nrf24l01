#include "spi_interface.h"

#include "pico/stdlib.h"
#include <string.h>

void spi_interface_init(spi_interface *self, spi_inst_t* spi, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    self->spi = spi;
    self->mosi = mosi;
    self->miso = miso;
    self->sck = sck;
    self->csn = csn;
    self->ce = ce;

     // Setup CSN pin
    gpio_init(csn);
    gpio_set_dir(csn, GPIO_OUT);
    gpio_put(csn, 1);

    // Setup CE pin
    gpio_init(ce);
    gpio_set_dir(ce, GPIO_OUT);
    gpio_put(ce, 0);

    // Setup MOSI, MISO and SCK pins
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
}

void spi_interface_send_command(spi_interface *self, uint8_t command, uint8_t *data, uint data_length, uint8_t *output, uint output_length) {
    gpio_put(self->csn, 0);

    uint8_t buffer[1 + 32];
    buffer[0] = command;
    if (data_length > 0) {
        memcpy(&buffer[1], data, data_length);
    }

    // Write the command and the data
    spi_write_blocking(self->spi, buffer, data_length + 1);

    // Read the output
    if (output_length > 0) {
        spi_read_blocking(self->spi, 0xFF, output, output_length);
    }

    gpio_put(self->csn, 1);
}

void spi_interface_pulse_ce(spi_interface *self) {
    gpio_put(self->ce, 1);
    sleep_us(15);
    gpio_put(self->ce, 0);
}

void spi_interface_enable_ce(spi_interface *self) {
    gpio_put(self->ce, 1);
}

void spi_interface_disable_ce(spi_interface *self) {
    gpio_put(self->ce, 0);
}
