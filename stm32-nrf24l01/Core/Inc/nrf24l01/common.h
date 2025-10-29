#pragma once

#include <stdint.h>

static void sleep_us(uint32_t us)
{
    uint32_t start = SysTick->VAL;
    uint32_t ticks_per_us = SystemCoreClock / 1000000;
    uint32_t ticks_to_wait = us * ticks_per_us;
    uint32_t reload = SysTick->LOAD + 1;
    uint32_t elapsed = 0;

    while (elapsed < ticks_to_wait)
    {
        uint32_t now = SysTick->VAL;

        // SysTick is a down-counter
        if (start >= now)
            elapsed += start - now;
        else
            elapsed += start + (reload - now);

        start = now;
    }
}
