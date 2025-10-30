#include "stm32_hal.h"

#include "stm32f1xx_hal.h"

void stm32_hal_write_pin(void *port, uint16_t pin, bool value) {
    GPIO_PinState state = value ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(port, pin, state);
}

uint8_t stm32_hal_spi_transmit(void *spi, const uint8_t *data, uint16_t size, uint32_t timeout) {
    return HAL_SPI_Transmit(spi, data, size, timeout);
}

uint8_t stm32_hal_spi_receive(void *spi, uint8_t *data, uint16_t size, uint32_t timeout) {
    return HAL_SPI_Receive(spi, data, size, timeout);
}

uint32_t stm32_hal_get_sys_tick_val() {
    return SysTick->VAL;
}

uint32_t stm32_hal_get_sys_tick_load() {
    return SysTick->LOAD;
}

uint32_t stm32_hal_get_system_core_clock() {
    return SystemCoreClock;
}

uint32_t stm32_hal_get_tick() {
    return HAL_GetTick();
}
