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

void nrf24l01_power_up(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config |= 0b00000010;
    register_map_write_register(&device->register_map, 0x00, &config, 1);

    // Wait for the Tpd2stby delay
    sleep_us(1500);
}

void nrf24l01_set_as_primary_tx(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config &= 0b11111110;
    register_map_write_register(&device->register_map, 0x00, &config, 1);
}

void nrf24l01_set_as_primary_rx(nrf24l01 *device) {
    // Read the config register
    uint8_t config;
    register_map_read_register(&device->register_map, 0x00, &config, 1);

    // Edit the config byte and write it
    config |= 0b00000001;
    register_map_write_register(&device->register_map, 0x00, &config, 1);
}

void nrf24l01_config_tx(nrf24l01 *device, uint8_t *value) {
    // Write TX_ADDR
    register_map_write_register(&device->register_map, 0x10, value, 5);

    // Write RX_ADDR_P0
    register_map_write_register(&device->register_map, 0x0A, value, 5);
}

void nrf24l01_config_rx(nrf24l01 *device, uint8_t *value) {
    // Write RX_ADDR_P1
    register_map_write_register(&device->register_map, 0x0B, value, 5);

    // Write RX_PW_Px
    uint8_t len = 32;
    register_map_write_register(&device->register_map, 0x11, &len, 1);
}
