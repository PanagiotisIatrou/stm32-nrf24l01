#pragma once

#include <stdint.h>
#include <stdbool.h>

void stm32_hal_write_pin(void *port, uint16_t pin, bool value);

uint8_t stm32_hal_spi_transmit(void *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

uint8_t stm32_hal_spi_receive(void *spi, uint8_t *data, uint16_t size, uint32_t timeout);

uint32_t stm32_hal_get_sys_tick_val();

uint32_t stm32_hal_get_sys_tick_load();

uint32_t stm32_hal_get_system_core_clock();

uint32_t stm32_hal_get_tick();
