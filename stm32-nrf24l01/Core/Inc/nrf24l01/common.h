#pragma once

#include <stdint.h>

/**
 * Busy wait sleep for the specified amount of ticks.
 * @param ticks The amount of ticks to sleep for.
 */
void sleep_ticks(uint32_t ticks);

/**
 * Busy wait sleep for the specified amount of microseconds.
 * @param us The amount of microseconds to sleep.
 */
void sleep_us(uint32_t us);

/**
 * Busy wait sleep for the specified amount of milliseconds.
 * @param ms The amount of milliseconds to sleep.
 */
void sleep_ms(uint32_t ms);
