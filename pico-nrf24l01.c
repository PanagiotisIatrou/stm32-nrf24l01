#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

typedef struct nrf24l01 {
    uint8_t mosi;
    uint8_t miso;
    uint8_t sck;
    uint8_t csn;
    uint8_t ce;
} nrf24l01;

nrf24l01 nrf24l01_init(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    nrf24l01 device = {
        .mosi = mosi,
        .miso = miso,
        .sck = sck,
        .csn = csn,
        .ce = ce,
    };

    // Setup CSN pin
    gpio_init(csn);
    gpio_set_dir(csn, GPIO_OUT);
    gpio_put(csn, 1);

    // Setup CE pin
    gpio_init(ce);
    gpio_set_dir(ce, GPIO_OUT);
    gpio_put(ce, 0);

    // Setup MISO, SCK and MOSI pins
    spi_init(spi0, 1000000);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    return device;
}

uint8_t nrf24l01_read_config(nrf24l01 device) {
    gpio_put(device.csn, 0);
    uint8_t cmd = 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    uint8_t config = 0;
    spi_read_blocking(spi0, 0x00, &config, 1);
    gpio_put(device.csn, 1);

    return config;
}

void nrf24l01_write_config(nrf24l01 device, uint8_t value) {
    gpio_put(device.csn, 0);
    uint8_t cmd = 0x20 | 0x00;
    spi_write_blocking(spi0, &cmd, 1);
    spi_write_blocking(spi0, &value, 1);
    gpio_put(device.csn, 1);
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

    nrf24l01 device0 = nrf24l01_init(19, 16, 18, 14, 15);

    // Read register
    uint8_t config = nrf24l01_read_config(device0);
    printf("Value: %d\n", config);

    // Write PWR_UP = 1 to the register
    config |= 0b00000010;
    nrf24l01_write_config(device0, config);

    // Read register
    config = nrf24l01_read_config(device0);
    printf("Value: %d\n", config);



    nrf24l01 device1 = nrf24l01_init(3, 4, 2, 12, 13);

    // Read register
    config = nrf24l01_read_config(device1);
    printf("Value: %d\n", config);

    // Write PWR_UP = 1 to the register
    config |= 0b00000010;
    nrf24l01_write_config(device1, config);

    // Read register
    config = nrf24l01_read_config(device1);
    printf("Value: %d\n", config);
}
