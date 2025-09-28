#include "nrf24l01.h"

#include "pico/stdlib.h"
#include "common.h"

bool nrf24l01_init(nrf24l01 *device, uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t ce) {
    // Check if the provided SPI pin combination is valid
    device->spi = pins_to_spi(mosi, miso, sck);
    if (device->spi == NULL) {
        return false;
    }

    // Initialize SPI
    if (device->spi == spi0 && !initialized_spi0) {
        initialized_spi0 = true;
        spi_init(device->spi, 1000000);
    } else if (device->spi == spi1 && !initialized_spi1) {
        initialized_spi1 = true;
        spi_init(device->spi, 1000000);
    }

    // Initialize the device struct
    device->mosi = mosi,
    device->miso = miso,
    device->sck = sck,
    device->csn = csn,
    device->ce = ce,

    // Setup CSN pin
    gpio_init(csn);
    gpio_set_dir(csn, GPIO_OUT);
    gpio_put(csn, 1);

    // Setup CE pin
    gpio_init(ce);
    gpio_set_dir(ce, GPIO_OUT);
    gpio_put(ce, 0);

    // Setup MISO, SCK and MOSI pins
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    // Initialize the register map
    register_map_init(&device->register_map, device->csn, device->spi);

    return true;
}

void nrf24l01_read_pwr_up(nrf24l01 *device, bool *value) {
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);
    *value = (config & 0b00000010) >> 1;
}

void nrf24l01_write_pwr_up(nrf24l01 *device, bool value) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    if (value) {
        config |= 0b00000010;
    } else {
        config &= 0b11111101;
    }
    register_map_write_register(&device->register_map, 0x00, &config, 1);
}
