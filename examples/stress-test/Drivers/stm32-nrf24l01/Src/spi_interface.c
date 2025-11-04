#include "spi_interface.h"

#include <string.h>

#include "nrf24l01_hal.h"

void spi_interface_init(
        spi_interface *self, void *spi, void *csn_port, uint16_t csn_pin, void *ce_port,
        uint16_t ce_pin) {
    self->spi = spi;
    self->csn_port = csn_port;
    self->csn_pin = csn_pin;
    self->ce_port = ce_port;
    self->ce_pin = ce_pin;

    // Set CSN to 1 and CE to 0
    nrf24l01_hal_write_pin(self->csn_port, self->csn_pin, 1);
    nrf24l01_hal_write_pin(self->ce_port, self->ce_pin, 0);
}

void spi_interface_send_command(
        spi_interface *self, uint8_t command, uint8_t *data, uint32_t data_length, uint8_t *output,
        uint32_t output_length) {
    nrf24l01_hal_write_pin(self->csn_port, self->csn_pin, 0);

    uint8_t buffer[1 + 32];
    buffer[0] = command;
    if (data_length > 0) {
        memcpy(&buffer[1], data, data_length);
    }

    // Write the command and the data
    nrf24l01_hal_spi_transmit(self->spi, buffer, data_length + 1, UINT32_MAX);

    // Read the output
    if (output_length > 0) {
        nrf24l01_hal_spi_receive(self->spi, output, output_length, UINT32_MAX);
    }

    nrf24l01_hal_write_pin(self->csn_port, self->csn_pin, 1);
}

void spi_interface_pulse_ce(spi_interface *self) {
    nrf24l01_hal_write_pin(self->ce_port, self->ce_pin, 1);
    nrf24l01_hal_sleep_us(15);
    nrf24l01_hal_write_pin(self->ce_port, self->ce_pin, 0);
}

void spi_interface_enable_ce(spi_interface *self) {
    nrf24l01_hal_write_pin(self->ce_port, self->ce_pin, 1);
}

void spi_interface_disable_ce(spi_interface *self) {
    nrf24l01_hal_write_pin(self->ce_port, self->ce_pin, 0);
}
