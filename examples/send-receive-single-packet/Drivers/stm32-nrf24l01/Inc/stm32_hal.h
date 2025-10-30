#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Writes 0 or 1 to the specified pin at the specified port.
 * @param port The port GPIOx of the pin.
 * @param pin The pin number.
 * @param value The value to write (0 or 1)
 */
void stm32_hal_write_pin(void *port, uint16_t pin, bool value);

/**
 * Transmits an array of bytes through the specified SPI interface.
 * @param spi Pointer to the SPI interface to use. Ex. &hspi1.
 * @param data The array of bytes to transmit.
 * @param size The number of bytes to transmit.
 * @param timeout The amount of milliseconds to timeout after.
 * @return The status of the transmission.
 */
uint8_t stm32_hal_spi_transmit(void *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * Receives an array of bytes through the specified SPI interface.
 * @param spi Pointer to the SPI interface to use. Ex. &hspi1.
 * @param data A array buffer of bytes to receive.
 * @param size The number of bytes to receive.
 * @param timeout The amount of milliseconds to timeout after.
 * @return The status of the reception.
 */
uint8_t stm32_hal_spi_receive(void *spi, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * @return The value of the SysTick (probably something like SysTick->VAL).
 */
uint32_t stm32_hal_get_sys_tick_val();

/**
 * @return The reload value of the SysTick (probably something like SysTick->LOAD).
 */
uint32_t stm32_hal_get_sys_tick_load();

/**
 * @return The clock of the CPU in Hz (probably something like SystemCoreClock).
 */
uint32_t stm32_hal_get_system_core_clock();

/**
 *
 * @return The current tick count in milliseconds (probably something like HAL_GetTick())
 */
uint32_t stm32_hal_get_tick();
