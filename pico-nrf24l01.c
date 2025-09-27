#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

uint8_t read_config() {
    gpio_put(17, 0);
    uint8_t cmd = 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    uint8_t config = 0;
    spi_read_blocking(spi0, 0x00, &config, 1);
    gpio_put(17, 1);

    return config;
}

void write_config(uint8_t value) {
    gpio_put(17, 0);
    uint8_t cmd = 0x20 | 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    spi_write_blocking(spi0, &value, 1);
    gpio_put(17, 1);
}

int main()
{
    // Enable stdio and wait for serial monitor to connect
    stdio_init_all();
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    sleep_ms(1000);
    printf("Starting...\n");

    // Wait for init
    sleep_ms(1000);

    // Setup CSN pin manually
    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_put(17, 1);

    // Setup CE pin
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_put(0, 0);

    // Setup MISO, SCK and MOSI pins
    spi_init(spi0, 5000000);
    gpio_set_function(16, GPIO_FUNC_SPI); // MISO
    gpio_set_function(18, GPIO_FUNC_SPI); // SCK
    gpio_set_function(19, GPIO_FUNC_SPI); // MOSI

    // Read register
    uint8_t config = read_config();
    printf("Value: %d\n", config);

    // Write PWR_UP = 1 to the register
    config |= 0b00000010;
    write_config(config);

    // Read register
    config = read_config();
    printf("Value: %d\n", config);
}
