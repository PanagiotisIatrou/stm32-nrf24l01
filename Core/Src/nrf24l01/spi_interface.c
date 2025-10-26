#include "../../Inc/nrf24l01/spi_interface.h"

#include <string.h>

#include "../../Inc/nrf24l01/common.h"

void spi_interface_init(
        spi_interface *self, SPI_HandleTypeDef *spi, GPIO_TypeDef *csn_port, uint16_t csn_pin, GPIO_TypeDef *ce_port,
        uint16_t ce_pin) {
    self->spi = spi;
    self->csn_port = csn_port;
    self->csn_pin = csn_pin;
    self->ce_port = ce_port;
    self->ce_pin = ce_pin;

    // Set CSN to 1 and CE to 0
    HAL_GPIO_WritePin(self->csn_port, self->csn_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(self->ce_port, self->ce_pin, GPIO_PIN_RESET);
}

void spi_interface_send_command(
        spi_interface *self, uint8_t command, uint8_t *data, uint32_t data_length, uint8_t *output,
        uint32_t output_length) {
    HAL_GPIO_WritePin(self->csn_port, self->csn_pin, GPIO_PIN_RESET);

    uint8_t buffer[1 + 32];
    buffer[0] = command;
    if (data_length > 0) {
        memcpy(&buffer[1], data, data_length);
    }

    // Write the command and the data
    HAL_SPI_Transmit(self->spi, buffer, data_length + 1, HAL_MAX_DELAY);

    // Read the output
    if (output_length > 0) {
        HAL_SPI_Receive(self->spi, output, output_length, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(self->csn_port, self->csn_pin, GPIO_PIN_SET);
}

void spi_interface_pulse_ce(spi_interface *self) {
    HAL_GPIO_WritePin(self->ce_port, self->ce_pin, GPIO_PIN_SET);
    sleep_us(15);
    HAL_GPIO_WritePin(self->ce_port, self->ce_pin, GPIO_PIN_RESET);
}

void spi_interface_enable_ce(spi_interface *self) { HAL_GPIO_WritePin(self->ce_port, self->ce_pin, GPIO_PIN_SET); }

void spi_interface_disable_ce(spi_interface *self) { HAL_GPIO_WritePin(self->ce_port, self->ce_pin, GPIO_PIN_RESET); }
