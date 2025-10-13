#pragma once

#include "hardware/spi.h"

/**
 * SPI peripheral abstraction layer, used to communicate with the nRF24l01 device.
 */
typedef struct {
    spi_inst_t* spi;
    uint8_t mosi;
    uint8_t miso;
    uint8_t sck;
    uint8_t csn;
    uint8_t ce;
} spi_interface;

/**
 * Initializes the SPI interface abstraction layer and initializes the corresponding
 * GPIO pins. Requires the SPI peripheral to be initialized beforehand.
 * @param self The spi_interface struct to initialize.
 * @param spi The SPI peripheral to use.
 * @param mosi The SPI MOSI GPIO pin.
 * @param miso The SPI MISO GPIO pin.
 * @param sck The SPI SCK GPIO pin.
 * @param csn The CSN GPIO pin.
 * @param ce The CE GPIO pin.
 */
void spi_interface_init(spi_interface *self, spi_inst_t* spi, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce);

/**
 * Sends a command to the nRF24l01 device.
 * @param self The spi_interface struct to act upon.
 * @param command The command byte to send.
 * @param data Pointer to the bytes to send after the command byte. Can be NULL if no data is to be sent.
 * @param data_length The number of bytes to send after the command byte. Can be 0 if no data is to be sent.
 * @param output Pointer to the buffer where the response bytes will be stored. Can be NULL if no response is needed.
 * @param output_length The number of bytes to read from the device. Can be 0 if no response is needed.
 */
void spi_interface_send_command(spi_interface *self, uint8_t command, uint8_t *data, uint data_length, uint8_t *output, uint output_length);

/**
 * Toggles the CE pin for at least 10us (15us to be safe).
 * @param self The spi_interface struct to act upon.
 */
void spi_interface_pulse_ce(spi_interface *self);

/**
 * Sets the CE pin to high.
 * @param self The spi_interface struct to act upon.
 */
void spi_interface_enable_ce(spi_interface *self);

/**
 * Sets the CE pin to low.
 * @param self The spi_interface struct to act upon.
 */
void spi_interface_disable_ce(spi_interface *self);
