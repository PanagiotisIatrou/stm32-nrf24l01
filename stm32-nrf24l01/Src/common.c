#include "common.h"

#include "stm32_hal.h"

void sleep_ticks(uint32_t ticks)
{
    uint32_t start = stm32_hal_get_sys_tick_val();
    uint32_t reload = stm32_hal_get_sys_tick_load() + 1;
    uint32_t elapsed = 0;

    while (elapsed < ticks)
    {
        uint32_t now = stm32_hal_get_sys_tick_val();
        if (start >= now)
            elapsed += start - now;
        else
            elapsed += start + (reload - now);
        start = now;
    }
}

void sleep_us(uint32_t us)
{
    sleep_ticks(us * (stm32_hal_get_system_core_clock() / 1000000));
}

void sleep_ms(uint32_t ms)
{
    sleep_ticks(ms * (stm32_hal_get_system_core_clock() / 1000));
}