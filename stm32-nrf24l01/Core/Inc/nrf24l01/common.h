#pragma once

#include <stdint.h>
#include "stm32f1xx_hal.h"

static void sleep_us(uint32_t us) {
    uint32_t cycles = (HAL_RCC_GetHCLKFreq() / 1000000L) * us;
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles) {
    }
}
