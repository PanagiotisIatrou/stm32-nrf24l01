#include "nrf24l01_hal.h"

#include "stm32f1xx_hal.h"

void nrf24l01_hal_write_pin(void *port, uint16_t pin, bool value) {
    GPIO_PinState state = value ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(port, pin, state);
}

uint8_t nrf24l01_hal_spi_transmit(void *spi, const uint8_t *data, uint16_t size, uint32_t timeout) {
    return HAL_SPI_Transmit(spi, data, size, timeout);
}

uint8_t nrf24l01_hal_spi_receive(void *spi, uint8_t *data, uint16_t size, uint32_t timeout) {
    return HAL_SPI_Receive(spi, data, size, timeout);
}

static void sleep_ticks(uint32_t ticks)
{
    uint32_t start = SysTick->VAL;
    uint32_t reload = SysTick->LOAD + 1;
    uint32_t elapsed = 0;

    while (elapsed < ticks)
    {
        uint32_t now = SysTick->VAL;
        if (start >= now)
            elapsed += start - now;
        else
            elapsed += start + (reload - now);
        start = now;
    }
}

void nrf24l01_hal_sleep_ms(uint32_t ms) {
    sleep_ticks(ms * (SystemCoreClock / 1000));
}

void nrf24l01_hal_sleep_us(uint32_t us) {
    sleep_ticks(us * (SystemCoreClock / 1000000));
}

uint32_t nrf24l01_hal_get_ms_ticks() {
    return HAL_GetTick();
}
