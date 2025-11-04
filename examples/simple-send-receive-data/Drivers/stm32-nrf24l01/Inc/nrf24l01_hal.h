#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Writes 0 or 1 to the specified pin at the specified port.
 * @param port The port GPIOx of the pin (NULL if not needed).
 * @param pin The pin number.
 * @param value The value to write (0 or 1)
 */
void nrf24l01_hal_write_pin(void *port, uint16_t pin, bool value);

/**
 * Transmits an array of bytes through the specified SPI interface.
 * @param spi Pointer to the SPI interface to use. Ex. &hspi1.
 * @param data The array of bytes to transmit.
 * @param size The number of bytes to transmit.
 * @param timeout The amount of milliseconds to timeout after.
 * @return The status of the transmission.
 */
uint8_t nrf24l01_hal_spi_transmit(void *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * Receives an array of bytes through the specified SPI interface.
 * @param spi Pointer to the SPI interface to use. Ex. &hspi1.
 * @param data A array buffer of bytes to receive.
 * @param size The number of bytes to receive.
 * @param timeout The amount of milliseconds to timeout after.
 * @return The status of the reception.
 */
uint8_t nrf24l01_hal_spi_receive(void *spi, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * Sleeps for the specified number of milliseconds.
 * @param ms The number of milliseconds to sleep.
 */
void nrf24l01_hal_sleep_ms(uint32_t ms);

/**
 * Sleeps for the specified number of microseconds.
 * @param us The number of microseconds to sleep.
 */
void nrf24l01_hal_sleep_us(uint32_t us);

/**
 * @return The number of milliseconds since the system started.
 */
uint32_t nrf24l01_hal_get_ms_ticks();
